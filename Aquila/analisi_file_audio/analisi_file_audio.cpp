// analisi_file_audio.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"
#include "aquila\global.h"
#include "aquila\source\WaveFile.h"
#include "aquila\transform\FftFactory.h"
#include "aquila/source/window/HammingWindow.h"
#include "aquila/transform/Mfcc.h"
#include <iostream>
#include <fstream>
#include <map>
#include <utility>

using namespace std;
using namespace Aquila;

// procedura per la scrittura dei risultati sul file csv
void scrittura(string nome, double *energia, SampleType *sommaMFCC, SampleType *mediaMFCC, 
	SampleType *maxMFCC, SampleType *minMFCC, double *f0, int *posFreqFond, double *freqFond, 
	double *pendenze, /*double *pitch,*/ size_t nframe, ofstream & fileOutput) {

	fileOutput << nome << ";energia;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (size_t)(energia[i]) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";sommaMFCC;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (int)(sommaMFCC[i] + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";mediaMFCC;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (int)(mediaMFCC[i] + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";maxMFCC;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (int)(maxMFCC[i] + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";minMFCC;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (int)(minMFCC[i] + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";f0;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (int)(f0[i] + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";posFreqFond;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (int)(posFreqFond[i]) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";freqFond;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (int)(freqFond[i] + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";pendenze;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (int)(pendenze[i] + 0.5) << ";";
	}
	fileOutput << endl;

	/*fileOutput << nome << ";pitch;";
	for (size_t i = 0; i < nfonemi; i++) {
		fileOutput << (size_t)(pitch[i] + 0.5) << ";";
	}
	fileOutput << endl;*/

	// stampe necessarie per arrivare a 12 (utili per non perdersi in noiosi inserimenti per creare i grafici su excel)
	fileOutput << endl << endl << endl;
}

SampleType* calcoloFinestra(SampleType *arr, HammingWindow hwindow, int length) {
	SampleType *finestra = new SampleType[length];
	for (int i = 0; i < length; i++) {
		finestra[i] = arr[i] * hwindow.sample(i);
	}
	return finestra;
}

double calcoloEnergia(SampleType *finestra, int length) {
	double energia = 0.;
	for (int i = 0; i < length; i++) {
		energia += finestra[i] * finestra[i];
	}
	return energia;
}

// algoritmo per la normalizzazione di un array di double
double* normalizza(double *arr, int lunghezza, double *picchi, int lunghezzaFrameInMs) {
	double *normalizzato = new double[lunghezza];
	int j = 0;
	for (int i = 0; i < lunghezza; i++) {
		normalizzato[i] = (arr[i] / picchi[j]);
		if ((((i + 1) * lunghezzaFrameInMs) % 2000 == 0) && i != 0)
			j++;
	}
	return normalizzato;
}

// algoritmo per ridurre il numero dei campioni di un fonema in dim potenza di 2 facendo una semplice media
SampleType* campioniToDim(SampleType *arr, size_t l, int dim) {
	SampleType *ris = new SampleType[dim];
	int campioni = l / dim;
	if (campioni == 0) {
		for (int i = 0; i < l; i++) {
			ris[i] = arr[i];
		}
		for (int i = l; i < dim; i++) {
			ris[i] = 0;
		}
	}
	else {
		int j = 0;
		int somma = 0;
		int ncampionifinali = 0;
		for (int i = 0; i < l; i++) {
			if (j < (dim - 1)) {
				if (i % campioni == 0 && i != 0) {
					ris[j] = somma / campioni;
					j++;
				}
				else {
					somma += arr[i];
				}
			}
			else {
				somma += arr[i];
				ncampionifinali++;
			}
		}
		ris[j] = somma / ncampionifinali;
	}
	return ris;
}

SampleType sommaDifferenze(SampleType *s, int k, size_t l) {
	SampleType somma = 0;
	for (int m = k; m < l; m++) {
		somma += abs(s[m] - s[m - k]);
	}
	return somma;
}
// calcolo della Short-time Average Magnitude Difference Function
SampleType* AMDF(SampleType *s, size_t N) {
	SampleType *amdf = new SampleType[N];
	for (int k = 0; k < N; k++) {
		amdf[k] = sommaDifferenze(s, k, N);
	}
	return amdf;
}

int ricercaMassimo(SampleType *arr, size_t l) {
	int sm = 1;
	SampleType massimo = arr[1];
	for (int i = 2; i < l; i++) {
		if (arr[i] > massimo) {
			massimo = arr[i];
			sm = i;
		}
	}
	return sm;
}

// algoritmo per l'analisi basato sull'estrazione di caratteristiche in ogni singolo frame
void analisi(int numeroFile, string percorso, int lunghezzaFrameInMs, ofstream & fileOutput) {
	WaveFile wav(percorso);
	FrequencyType sampleFrequency = wav.getSampleFrequency();
	string nome = wav.getFilename().erase(0, 64);
	// numero dei campioni del file audio
	double SIZE = wav.getSamplesCount();
	// lunghezza in ms del file audio
	double audioLength = wav.getAudioLength();
	// numero dei campioni nella finestra temporale
	size_t SAMPLES_IN_FRAME = (SIZE / audioLength) * lunghezzaFrameInMs;
	cout << "[" << numeroFile << "] " << nome << " (" << SAMPLES_IN_FRAME
		<< " samples in " << lunghezzaFrameInMs << " ms)";
	//  numero dei frame che contengono un numero di campioni in lunghezzaFrameInMs
	size_t nframe = audioLength / lunghezzaFrameInMs;
	SampleType *arr = new SampleType[SAMPLES_IN_FRAME];
	HammingWindow hwindow(SAMPLES_IN_FRAME);
	double *energia = new double[nframe];
	SampleType *sommaMFCC = new SampleType[nframe];
	SampleType *mediaMFCC = new SampleType[nframe];
	SampleType *maxMFCC = new SampleType[nframe];
	SampleType *minMFCC = new SampleType[nframe];
	double *f0 = new double[nframe];
	int *posFreqFond = new int[nframe];
	double *freqFond = new double[nframe];
	double *pendenze = new double[nframe];
	double *pitch = new double[nframe];
	// iteratore dell'array di scrittura
	int c = 0;
	int dim = 64;
	int iteratore = 0;

	SampleType max = wav.sample(0);
	/* rierca del valore massimo che sarà utile per la normalizzazione del file */
	/*for (size_t i = 1; i < SIZE; i++) {
		if (wav.sample(i) > max) {
			max = wav.sample(i);
		}
	}*/
	// iteratore per picchi
	int j = 0;
	SampleType *picchi = new SampleType[nframe];
	/* ogni 2 secondi si cambia il valore massimo per la normalizzazione */
	for (size_t i = 1; i < SIZE; i++)
	{
		SampleType posCorrente = wav.sample(i);
		if (posCorrente > max) {
			max = posCorrente;
		}
		if ((i % 88200 == 0) || i == SIZE - 1) {
			picchi[j] = max;
			max = posCorrente;
			j++;
		}
	}
	j = 0;

	/* il file viene scomposto in blocchi di lunghezza pari a lunghezzaFrameinMs e vengono estratte le caratteristiche
	 * presenti in cascun frame. Ogni campione è normalizzato rispetto al valore massimo all'interno del file
	 */
	for (size_t i = 0; i < SIZE; i++)
	{
		SampleType posCorrNorm = (wav.sample(i) / picchi[j]) * 1000;
		// se sto a un multiplo di lunghezzaFrameInMs aggiorno gli array con i valori ottenuti (vedi else)
		if ((i % SAMPLES_IN_FRAME == 0 || i == SIZE - 1) && i != 0)
		{
			arr[iteratore] = posCorrNorm;
			SampleType *finestra = calcoloFinestra(arr, hwindow, SAMPLES_IN_FRAME);
			energia[c] = calcoloEnergia(finestra, SAMPLES_IN_FRAME);
			SampleType *segnaleCampioni = campioniToDim(finestra, SAMPLES_IN_FRAME, dim);
			SignalSource *segnale = new SignalSource(segnaleCampioni, dim, sampleFrequency);

			// estrazione mfcc
			Aquila::Mfcc mfcc(segnale->getSamplesCount());
			auto mfccValues = mfcc.calculate(*segnale);
			SampleType somma = 0.;
			SampleType massimo = 0.;
			SampleType minimo = 0.;
			for (vector<SampleType>::iterator p = mfccValues.begin(); p != mfccValues.end(); p++) {
				somma += *p;
				if (*p > massimo)
					massimo = *p;
				if (*p < minimo)
					minimo = *p;
			}
			sommaMFCC[c] = somma;
			mediaMFCC[c] = (somma / mfccValues.size());
			maxMFCC[c] = massimo;
			minMFCC[c] = minimo;
			mfccValues.~vector();
			delete segnale;

			// estrazione caratteristiche dallo spettro
			auto fft = FftFactory::getFft(dim);
			SpectrumType spettro = fft->fft(segnaleCampioni);
			f0[c] = abs(spettro.at(0));
			int pos = 0;
			double frequenzaFond = abs(spettro.at(0));
			for (int a = 1; a < (dim / 2); a++)
			{
				if (abs(spettro.at(a)) > frequenzaFond) {
					pos = a;
					frequenzaFond = abs(spettro.at(a));
				}
			}
			posFreqFond[c] = pos;
			freqFond[c] = abs(spettro.at(pos));
			// il valore della pendenza è dato come la differenza tra il valore della frequenza
			// fondamentale e il valore della media tra la precedente e la successiva
			if (pos == 0)
				pendenze[c] = (abs(spettro.at(pos)) - abs(spettro.at(pos + 1)));
			else
				pendenze[c] = (((abs(spettro.at(pos)) - abs(spettro.at(pos - 1))) +
				(abs(spettro.at(pos)) - abs(spettro.at(pos + 1)))) / 2 );

			// stima della F0
			/*SampleType *amdf = AMDF(finestra, lunghezzaFonema);
			// in teoria: la periodicità (km) di un segnale periodico si ricava andando a cercare il secondo minimo 
			// della amdf; in pratica in questo caso come risultato da sempre l'ultimo valore della amdf
			// come informazione allora prendo il valore del massimo, che è unico all'interno del fonema
			int km = ricercaMassimo(amdf, lunghezzaFonema);
			// F0 è il rapporto tra la frequenza di campionamento del segnale e km
			pitch[c] = sampleFrequency / km;
			delete[] amdf;*/

			arr = new SampleType[SAMPLES_IN_FRAME];
			iteratore = 0;
			c++;
		}
		// aggiorno i valori delle singole caratteristiche nel frame corrente 
		else
		{
			arr[iteratore] = posCorrNorm;
			iteratore++;
		}
		if ((i % 88200 == 0) || i == SIZE - 1) {
			j++;
		}
		// stampa per la visualizzazione dell'avanzamento del processo
		// fa una stampa ogni secondo del file audio processato
		if (i % 44100 == 0)
		{
			double pos = i, size = SIZE;
			cout << "\r[" << numeroFile << "] " << nome << " (" << SAMPLES_IN_FRAME
				<< " samples in " << lunghezzaFrameInMs << " ms): "
				<< (int)((pos / size) * 100) << "%";
		}
	}

	/* scrittura dei valori delle caratteristiche nel file di output */
	scrittura(nome, energia, sommaMFCC, mediaMFCC, maxMFCC, minMFCC, 
		f0, posFreqFond, freqFond, pendenze, /*pitch,*/ nframe, fileOutput);

	cout << "\r[" << numeroFile << "] " << nome << " (" << SAMPLES_IN_FRAME
		<< " samples in " << lunghezzaFrameInMs << " ms): 100%\n";
}

// metodo main
int main(int argc, char * argv[])
{
	if (argc < 4)
	{
		cout << "parametri insufficienti [" << argc << "]" << endl;
		return 1;
	}

	int lunghezzaFrameInMs = atoi(argv[1]);
	int numeroFile = 1;
	ifstream risorse(argv[2]);
	ofstream fileOutput(argv[3]);
	fileOutput << "nome_file;caratteristiche" << endl;
	char ch;
	string s, percorso = "";
	// finchè non sono alla fine del file
	while (!risorse.eof())
	{
		risorse.get(ch);
		// estraggo la stringa con il percorso
		if (ch != '\n')
		{
			s = ch;
			percorso.append(s);
		}
		// una volta ottenuto il percorso analizzo il file
		else
		{
			// stampa inutile, serve solo per non perdersi in frustranti inserimenti quando si creano i grafici su excel
			if (numeroFile == 1 || numeroFile == 7) {
				fileOutput << endl;
			}
			analisi(numeroFile, percorso, lunghezzaFrameInMs, fileOutput);
			numeroFile++;
			percorso = "";
		}
	}

	// l'ultima stringa ottenuta dal file delle risorse se non vuota contiene l'ultimo percorso
	if (!percorso.empty())
	{
		int size = percorso.size() - 1;
		percorso.erase(size, size - 1);
		analisi(numeroFile, percorso, lunghezzaFrameInMs, fileOutput);
	}

	risorse.close();
	fileOutput.close();

    return 0;
}