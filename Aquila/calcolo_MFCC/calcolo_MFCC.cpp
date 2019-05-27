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
void scrittura(string nome, SampleType *mfcc01, SampleType *mfcc02, SampleType *mfcc03,	SampleType *mfcc04,
	SampleType *mfcc05, SampleType *mfcc06, SampleType *mfcc07, SampleType *mfcc08,	SampleType *mfcc09, 
	SampleType *mfcc10, SampleType *mfcc11, SampleType *mfcc12, size_t nframe, ofstream & fileOutput) {

	fileOutput << nome << ";mfcc01;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (size_t)((mfcc01[i] / 1000) + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";mfcc02;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (size_t)((mfcc02[i] / 1000) + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";mfcc03;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (size_t)((mfcc03[i] / 1000) + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";mfcc04;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (size_t)((mfcc04[i] / 1000) + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";mfcc05;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (size_t)((mfcc05[i] / 1000) + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";mfcc06;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (size_t)((mfcc06[i] / 1000) + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";mfcc07;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (size_t)((mfcc07[i] / 1000) + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";mfcc08;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (size_t)((mfcc08[i] / 1000) + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";mfcc09;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (size_t)((mfcc09[i] / 1000 )+ 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";mfcc10;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (size_t)((mfcc10[i] / 1000) + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";mfcc11;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (size_t)((mfcc11[i] / 1000) + 0.5) << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";mfcc12;";
	for (size_t i = 0; i < nframe; i++) {
		fileOutput << (size_t)((mfcc12[i] / 1000) + 0.5) << ";";
	}
	fileOutput << endl;

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
	SampleType *mfcc01 = new SampleType[nframe];
	SampleType *mfcc02 = new SampleType[nframe];
	SampleType *mfcc03 = new SampleType[nframe];
	SampleType *mfcc04 = new SampleType[nframe];
	SampleType *mfcc05 = new SampleType[nframe];
	SampleType *mfcc06 = new SampleType[nframe];
	SampleType *mfcc07 = new SampleType[nframe];
	SampleType *mfcc08 = new SampleType[nframe];
	SampleType *mfcc09 = new SampleType[nframe];
	SampleType *mfcc10 = new SampleType[nframe];
	SampleType *mfcc11 = new SampleType[nframe];
	SampleType *mfcc12 = new SampleType[nframe];
	// iteratore dell'array di scrittura
	int c = 0;
	int dim = 1024;
	int iteratore = 0;

	SampleType max = wav.sample(0);
	/* rierca del valore massimo che sarà utile per la normalizzazione del file */
	for (size_t i = 1; i < SIZE; i++) {
		if (wav.sample(i) > max) {
			max = wav.sample(i);
		}
	}

	/* il file viene scomposto in blocchi di lunghezza pari a lunghezzaFrameinMs e vengono estratte le caratteristiche
	* presenti in cascun frame
	*/
	for (size_t i = 0; i < SIZE; i++)
	{
		// normalizzazione del file tramite il valore del campione più alto
		SampleType posCorrNorm = (wav.sample(i) / max) * 100;
		// se sto a un multiplo di lunghezzaFrameInMs aggiorno gli array con i valori ottenuti (vedi else)
		if ((i % SAMPLES_IN_FRAME == 0 || i == SIZE - 1) && i != 0)
		{
			arr[iteratore] = posCorrNorm;
			SampleType *segnaleCampioni = campioniToDim(arr, SAMPLES_IN_FRAME, dim);
			SignalSource *segnale = new SignalSource(segnaleCampioni, dim, sampleFrequency);

			// estrazione mfcc
			Aquila::Mfcc mfcc(segnale->getSamplesCount());
			auto mfccValues = mfcc.calculate(*segnale);
			mfcc01[c] = mfccValues.at(0);
			mfcc02[c] = mfccValues.at(1);
			mfcc03[c] = mfccValues.at(2);
			mfcc04[c] = mfccValues.at(3);
			mfcc05[c] = mfccValues.at(4);
			mfcc06[c] = mfccValues.at(5);
			mfcc07[c] = mfccValues.at(6);
			mfcc08[c] = mfccValues.at(7);
			mfcc09[c] = mfccValues.at(8);
			mfcc10[c] = mfccValues.at(9);
			mfcc11[c] = mfccValues.at(10);
			mfcc12[c] = mfccValues.at(11);
			mfccValues.~vector();
			delete segnale;

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
	cout << "\r[" << numeroFile << "] " << nome << " (" << SAMPLES_IN_FRAME
		<< " samples in " << lunghezzaFrameInMs << " ms): 100%\n";

	/* scrittura dei valori delle caratteristiche nel file di output */
	scrittura(nome, mfcc01, mfcc02, mfcc03, mfcc04, mfcc05,
		mfcc06, mfcc07, mfcc08, mfcc09, mfcc10, mfcc11, mfcc12, nframe, fileOutput);
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