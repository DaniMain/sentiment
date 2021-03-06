// provaAMDF.cpp : definisce il punto di ingresso dell'applicazione console.

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

int ricercaSecondoMinimo(SampleType *arr, size_t l) {
	int sm = 1;
	SampleType minimo = arr[1];
	for (int i = 2; i < l; i++) {
		if (arr[i] < minimo) {
			minimo = arr[i];
			sm = i;
		}
	}
	return sm;
}

int ricercaMassimo(SampleType *arr, size_t l) {
	int sm = 1;
	SampleType max = arr[0];
	for (int i = 2; i < l; i++) {
		if (arr[i] > max) {
			max = arr[i];
			sm = i;
		}
	}
	return sm;
}

// algoritmo per l'analisi
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
	size_t nPicchi = (audioLength / 2000);
	double *picchiEnergia = new double[nPicchi];
	// iteratore dell'array di scrittura
	int c = 0;
	int j = 0;
	double piccoEnergia = 0.;
	int iteratore = 0;

	/* primo passo dell'analisi: scomposizione del file in blocchi di lunghezzaFrameInMs e normalizzazione */
	for (size_t i = 0; i < SIZE; i++)
	{
		SampleType posCorrente = wav.sample(i);
		// se sto a un multiplo di lunghezzaFrameInMs aggiorno gli array con i valori ottenuti (vedi else)
		if ((i % SAMPLES_IN_FRAME == 0 || i == SIZE - 1) && i != 0)
		{
			arr[iteratore] = posCorrente;
			SampleType *finestra = calcoloFinestra(arr, hwindow, SAMPLES_IN_FRAME);
			energia[c] = calcoloEnergia(finestra, SAMPLES_IN_FRAME);
			if (energia[c] > piccoEnergia)
				piccoEnergia = energia[c];
			// se sto a un multiplo di 2 sec resetto il valore del picco
			if ((i % 88200 == 0) || i == SIZE - 1) {
				picchiEnergia[j] = piccoEnergia;
				piccoEnergia = 0.;
				j++;
			}
			arr = new SampleType[SAMPLES_IN_FRAME];
			iteratore = 0;
			c++;
		}
		// aggiorno i valori delle singole caratteristiche nel frame corrente 
		else
		{
			arr[iteratore] = posCorrente;
			iteratore++;
		}
		// stampa per la visualizzazione dell'avanzamento del processo
		// fa una stampa ogni 2 secondi del file audio processato
		if (i % 88200 == 0)
		{
			double pos = i, size = SIZE;
			cout << "\r[" << numeroFile << "] " << nome << " (" << SAMPLES_IN_FRAME
				<< " samples in " << lunghezzaFrameInMs << " ms) normalizzazione e estrazione fonemi: "
				<< (int)((pos / size) * 100) << "%";
		}
	}
	double *energiaNormalizzata = normalizza(energia, nframe, picchiEnergia, lunghezzaFrameInMs);

	/* secondo passo: estrazione dei fonemi */
	size_t *indiciFonemiProv = new size_t[nframe];
	size_t *lunghezzeFonemiProv = new size_t[nframe];
	// mappa con chiave progressiva e con valori degli array che conterranno i campioni di ciascun fonema
	map<int, SampleType*> mappaFonemi;
	// iteratore per indiciFonemi
	c = 0;
	// iteratore per lunghezzeFonemi
	j = 0;
	size_t lunghezza = 0;
	// booleano per capire se si inizia con un fonema o meno
	bool iniziaFonema = true;
	if (energiaNormalizzata[0] < 0.15) {
		indiciFonemiProv[c] = 0;
		c++;
		iniziaFonema = false;
	}
	else {
		lunghezzeFonemiProv[j] = SAMPLES_IN_FRAME;
		indiciFonemiProv[c] = 0;
		c++;
	}
	// nello scandire i frame mi costriusco anche la mappa: come chiave ho un indice progressivo mentre come 
	// valore ho un array dinamico di lunghezza pari alla lunghezza del fonema
	for (size_t i = 1; i < nframe; i++) {
		// se l'energia normalizzata � inferiore a 0.15 non abbiamo fonemi
		if (energiaNormalizzata[i] < 0.15) {
			// se prima avevo un fonema aggiorno gli indici, nfonemi, lunghezzeFonemiProv e la mappa
			if (energiaNormalizzata[i - 1] >= 0.15) {
				indiciFonemiProv[c] = i;
				c++;
				lunghezzeFonemiProv[j] = lunghezza;
				mappaFonemi.insert(pair<int, SampleType*>(j, new SampleType[lunghezza]));
				j++;
			}
		}
		// se l'energia normalizzata � superiore a 0.15 abbiamo un fonema
		else {
			// se prima non avevo un fonema aggiorno gli indici e le lunghezze
			if (energiaNormalizzata[i - 1] < 0.15) {
				indiciFonemiProv[c] = i;
				c++;
				lunghezza = SAMPLES_IN_FRAME;
			}
			// se prima avevo un fonema aggiorno l'array delle lunghezze dei fonemi
			else {
				lunghezza += SAMPLES_IN_FRAME;
			}
		}
	}
	// numero dei fonemi
	size_t nfonemi = j;
	if (energiaNormalizzata[nframe - 1] > 0.15) {
		nfonemi++;
	}
	/* concludo l'estrazione dei fonemi creando l'array degli indici dei fonemi con la giusta lunghezza */
	size_t *indiciFonemi = new size_t[c];
	size_t *lunghezzeFonemi = new size_t[nfonemi];
	for (int i = 0; i < c; i++) {
		indiciFonemi[i] = indiciFonemiProv[i];
	}
	delete[] indiciFonemiProv;
	for (int i = 0; i < nfonemi; i++) {
		lunghezzeFonemi[i] = lunghezzeFonemiProv[i];
	}
	delete[] lunghezzeFonemiProv;
	cout << "\r[" << numeroFile << "] " << nome << " (" << SAMPLES_IN_FRAME
		<< " samples in " << lunghezzaFrameInMs << " ms) normalizzazione e estrazione fonemi: 100%\n";

	/* terzo passo: estrazione delle caratteristiche per ciascun fonema */
	bool fonema = iniziaFonema;
	size_t indiceInizioFonema = 0;
	size_t indiceFineFonema = 1;
	// se non si inizia con un fonema incremento di 1
	if (!iniziaFonema) {
		indiceInizioFonema++;
		indiceFineFonema++;
	}
	// iteratore per lunghezzeFonemi e mappaFonemi
	int k = 0;
	size_t lunghezzaFonema = lunghezzeFonemi[k];
	c = 0;
	SampleType *campioniFonema = mappaFonemi[k];
	SampleType posCorrente;
	// iteratore per campioniFonema
	iteratore = 0;
	int dim = 512;
	for (size_t i = 0; i < SIZE; i++) {
		// se sto all'interno di un fonema
		if (fonema) {
			posCorrente = wav.sample(i);
			// se sono arrivato alla fine del fonema
			if (i == ((indiciFonemi[indiceFineFonema]) * SAMPLES_IN_FRAME) || i == SIZE - 1) {
				SampleType *segnaleCampioni = campioniToDim(campioniFonema, lunghezzaFonema, dim);
				SignalSource *segnale = new SignalSource(segnaleCampioni, dim, sampleFrequency);

				// stima F0 usando ricercaSecondoMinimo e ricercaSecondoMassimo
				SampleType *amdf = AMDF(campioniFonema, lunghezzaFonema);
				int kmMin = ricercaSecondoMinimo(amdf, lunghezzaFonema);
				int kmMinMeta = ricercaSecondoMinimo(amdf, lunghezzaFonema / 2);
				int kmMax = ricercaMassimo(amdf, lunghezzaFonema);
				fileOutput << kmMin << ";" << sampleFrequency / kmMin << ";";
				fileOutput << kmMinMeta << ";" << sampleFrequency / kmMinMeta << ";";
				fileOutput << kmMax << ";" << sampleFrequency / kmMax << ";";
				fileOutput << endl;

				// reset
				iteratore = 0;
				k++;
				if (k < nfonemi) {
					lunghezzaFonema = lunghezzeFonemi[k];
					campioniFonema = mappaFonemi[k];
				}
				indiceInizioFonema += 2;
				indiceFineFonema += 2;
				fonema = false;
				c++;
			}
			// se non sono alla fine aggiorno l'array dei fonemi
			else {
				campioniFonema[iteratore] = posCorrente;
				iteratore++;
			}
		}
		// se sto all'inzio di un fonema
		if (i == ((indiciFonemi[indiceInizioFonema]) * SAMPLES_IN_FRAME)) {
			fonema = true;
			campioniFonema[iteratore] = wav.sample(i);
			iteratore++;
		}
		// stampa per la visualizzazione dell'avanzamento del processo
		// fa una stampa ogni 2 secondi del file audio processato
		if (i % 88200 == 0)
		{
			double pos = i, size = SIZE;
			cout << "\r[" << numeroFile << "] " << nome << " (" << SAMPLES_IN_FRAME
				<< " samples in " << lunghezzaFrameInMs << " ms) estrazione caratteristiche: "
				<< (int)((pos / size) * 100) << "%";
		}
	}
	cout << "\r[" << numeroFile << "] " << nome << " (" << SAMPLES_IN_FRAME
		<< " samples in " << lunghezzaFrameInMs << " ms) estrazione caratteristiche: 100%\n";
}

// metodo main
// (l'algoritmo vecchio sta in analisi_file_audio_1spf)
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
	fileOutput << "km secondoMin; f0 secondoMin;km secondoMinMet�;f0 secondoMinMet�;km Max;f0 Max;" << endl;
	char ch;
	string s, percorso = "";
	// finch� non sono alla fine del file
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