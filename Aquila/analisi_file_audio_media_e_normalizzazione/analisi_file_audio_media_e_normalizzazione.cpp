// analisi_file_audio.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"
#include "aquila\global.h"
#include "aquila\source\WaveFile.h"
#include "aquila\transform\FftFactory.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace Aquila;

// procedura per la scrittura dei risultati sul file audio
void scrittura(int LENGTH, string nome, int *discretizzatore, int *media,
	int *picchi, double *energia, int *frequenzeH0, int *frequenzeH1, int *frequenzeH2,
	int *frequenzaFond, int *posFreqFond, int *pendenze, int *sommatore, ofstream & fileOutput) {

	fileOutput << nome << ";discretizzatore;";
	int mediaVar = (sommatore[0] / LENGTH);
	for (int i = 0; i <= LENGTH; i++)
	{

		fileOutput << (discretizzatore[i] - mediaVar) << ";";
	}
	fileOutput << endl;

	mediaVar = (sommatore[1] / LENGTH);
	fileOutput << nome << ";media;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << (media[i] - mediaVar) << ";";
	}
	fileOutput << endl;

	mediaVar = (sommatore[2] / LENGTH);
	fileOutput << nome << ";picchi;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << (picchi[i] - mediaVar) << ";";
	}
	fileOutput << endl;

	mediaVar = (sommatore[3] / LENGTH);
	fileOutput << nome << ";energia;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << (energia[i] - mediaVar) << ";";
	}
	fileOutput << endl;

	mediaVar = (sommatore[4] / LENGTH);
	fileOutput << nome << ";frequenzeH0;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << (frequenzeH0[i] - mediaVar) << ";";
	}
	fileOutput << endl;

	mediaVar = (sommatore[5] / LENGTH);
	fileOutput << nome << ";frequenzeH1;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << (frequenzeH1[i] - mediaVar) << ";";
	}
	fileOutput << endl;

	mediaVar = (sommatore[6] / LENGTH);
	fileOutput << nome << ";frequenzeH2;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << (frequenzeH2[i] - mediaVar) << ";";
	}
	fileOutput << endl;

	mediaVar = (sommatore[7] / LENGTH);
	fileOutput << nome << ";frequenzaFond;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << (frequenzaFond[i] - mediaVar) << ";";
	}
	fileOutput << endl;

	mediaVar = (sommatore[8] / LENGTH);
	fileOutput << nome << ";posFreqFond;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << (posFreqFond[i] - mediaVar) << ";";
	}
	fileOutput << endl;

	mediaVar = (sommatore[9] / LENGTH);
	fileOutput << nome << ";pendenze;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << (pendenze[i] - mediaVar) << ";";
	}
	fileOutput << endl;

	// ci sono due campi vuoti per arrivare a 12
	fileOutput << endl << endl;
}

// algoritmo principale per l'analisi del file audio
void analisi(int numeroFile, string percorso, int lunghezzaFrameInMs, ofstream & fileOutput) {
	WaveFile wav(percorso);
	string nome = wav.getFilename().erase(0, 64);
	double SIZE = wav.getSamplesCount();
	double audioLength = wav.getAudioLength();
	size_t SAMPLES_IN_FRAME = (SIZE / audioLength) * lunghezzaFrameInMs;
	cout << "[" << numeroFile << "] " << nome << " (" << SAMPLES_IN_FRAME << " samples in " << lunghezzaFrameInMs << " ms)";
	int c = 0;
	int LENGTH = audioLength / lunghezzaFrameInMs;
	int *discretizzatore = new int[LENGTH];
	int *media = new int[LENGTH];
	int *picchi = new int[LENGTH];
	double *energia = new double[LENGTH];
	SpectrumType spettro;
	int *frequenzeH0 = new int[LENGTH];
	int *frequenzeH1 = new int[LENGTH];
	int *frequenzeH2 = new int[LENGTH];
	int *frequenzaFond = new int[LENGTH];
	int *posFreqFond = new int[LENGTH];
	int *pendenze = new int[LENGTH];

	/* per la normalizzazione del file */
	SampleType minValue = wav.sample(0), maxValue(0);
	for (auto it = wav.begin(); it != wav.end(); ++it)
	{
		if (*it < minValue)
			minValue = *it;
		if (*it > maxValue)
			maxValue = *it;
	}
	double scala = maxValue - minValue;

	int rabbia = 0;
	int calma = 0;
	int *classificazione = new int[LENGTH];
	int sommaDisc = 0, sommaMedia = 0, sommaPicchi = 0, sommaEner = 0,
	sommaF0 = 0, sommaF1 = 0, sommaF2 = 0, sommaFreqFond = 0, sommaPosFreqFond = 0, sommaPend = 0;
	SampleType somma = 0;
	SampleType picchio = ((wav.sample(0) - minValue) / scala);
	double energiaCampione = 0;
	SampleType *arr = new SampleType[SAMPLES_IN_FRAME];
	int iteratore = 0;
	for (size_t i = 0; i < SIZE; i++)
	{
		// se sto a un multiplo di lunghezzaFrameInMs aggiorno gli array con i valori ottenuti (vedi else)
		if ((i % SAMPLES_IN_FRAME == 0 || i == SIZE - 1) && i != 0)
		{
			SampleType campioneNormalizzato = ((wav.sample(i) - minValue) / scala) * 100;
			somma += campioneNormalizzato;
			if (campioneNormalizzato > picchio)
				picchio = campioneNormalizzato;
			energiaCampione += ((campioneNormalizzato)*(campioneNormalizzato));
			arr[iteratore] = campioneNormalizzato;
			discretizzatore[c] = somma + 0.5;
			media[c] = (somma / SAMPLES_IN_FRAME) + 0.5;
			picchi[c] = picchio + 0.5;
			energia[c] = (energiaCampione)+0.5;
			auto fft = FftFactory::getFft(64);
			SpectrumType spettro = fft->fft(arr);
			frequenzeH0[c] = abs(spettro.at(0)) + 0.5;
			frequenzeH1[c] = abs(spettro.at(1)) + 0.5;
			frequenzeH2[c] = abs(spettro.at(2)) + 0.5;
			int pos = 0;
			int freqFond = abs(spettro.at(0)) + 0.5;
			for (int j = 1; j < 32; j++)
			{
				if (abs(spettro.at(j)) > freqFond) {
					pos = j;
					freqFond = abs(spettro.at(j)) + 0.5;
				}
			}
			posFreqFond[c] = pos;
			frequenzaFond[c] = abs(spettro.at(pos)) + 0.5;
			// il valore della pendenza è dato come la differenza tra il valore della
			// frequenza fondamentale e il valore della frequenza precedente o successiva
			if (pos == 0)
				pendenze[c] = (abs(spettro.at(pos)) - abs(spettro.at(pos + 1))) + 0.5;
			else
				pendenze[c] = (__max((abs(spettro.at(pos)) - abs(spettro.at(pos - 1))), (abs(spettro.at(pos)) - abs(spettro.at(pos + 1))))) + 0.5;
			sommaDisc += discretizzatore[c];
			sommaMedia += media[c];
			sommaPicchi += picchi[c];
			sommaEner += energia[c];
			sommaF0 += frequenzeH0[c];
			sommaF1 += frequenzeH1[c];
			sommaF2 += frequenzeH2[c];
			sommaFreqFond += frequenzaFond[c];
			sommaPosFreqFond += posFreqFond[c];
			sommaPend += pendenze[c];

			/* analisi dei risultati ottenuti in base ai criteri illustrati nel file euristica.docx */
			// discretizzatore
			/*if (discretizzatore[c] > 1000000)
			rabbia += 1;
			else if (discretizzatore[c] > 2000000)
			rabbia += 2;
			else if (discretizzatore[c] > 5000000)
			rabbia += 3;
			else if (discretizzatore[c] > 10000000)
			rabbia += 4;
			else if (discretizzatore[c] < 200000)
			calma++;
			else if (discretizzatore[c] < 100000)
			calma += 2;
			// picchi
			if (picchi[c] > 17000)
			rabbia++;
			else if (picchi[c] > 20000)
			rabbia += 2;
			else if (picchi[c] > 30000)
			rabbia += 3;
			else if (picchi[c] < 17000)
			calma++;
			else if (picchi[c] < 10000)
			calma += 2;
			else if (picchi[c] < 5000)
			calma += 3;
			// energia
			if (energia[c] > 6E+16)
			rabbia++;
			else if (energia[c] > 1E+17)
			rabbia += 2;
			else if (energia[c] > 2E+17)
			rabbia += 3;
			else if (energia[c] > 5E+17)
			rabbia += 4;
			else if (energia[c] < 6E+16)
			calma++;
			else if (energia[c] < 2E+16)
			calma += 2;
			else if (energia[c] < 5E+15)
			calma += 3;
			// frequenzeH0
			if (frequenzeH0[c] > 150000)
			rabbia++;
			else if (frequenzeH0[c] > 300000)
			rabbia += 2;
			else if (frequenzeH0[c] > 500000)
			rabbia += 3;
			else if (frequenzeH0[c] < 50000)
			calma += 2;
			// frequenzeH1
			if (frequenzeH1[c] > 100000)
			rabbia++;
			else if (frequenzeH1[c] > 200000)
			rabbia += 2;
			else if (frequenzeH1[c] > 300000)
			rabbia += 3;
			else if (frequenzeH1[c] < 100000)
			calma++;
			else if (frequenzeH1[c] < 30000)
			calma += 2;
			// frequenzeH2
			if (frequenzeH2[c] > 50000)
			rabbia++;
			else if (frequenzeH2[c] > 100000)
			rabbia += 2;
			else if (frequenzeH2[c] > 200000)
			rabbia += 3;
			else if (frequenzeH2[c] < 50000)
			calma++;
			else if (frequenzeH2[c] < 30000)
			calma += 2;
			// frequenzaFond
			if (frequenzaFond[c] > 100000)
			rabbia++;
			else if (frequenzaFond[c] > 250000)
			rabbia += 2;
			else if (frequenzaFond[c] > 330000)
			rabbia += 3;
			else if (frequenzaFond[c] < 100000)
			calma++;
			else if (frequenzaFond[c] < 50000)
			calma += 2;
			// pendenze
			if (pendenze[c] > 100000)
			rabbia++;
			else if (pendenze[c] > 250000)
			rabbia += 2;
			else if (pendenze[c] > 330000)
			rabbia += 3;
			else if (pendenze[c] < 100000)
			calma++;
			else if (pendenze[c] < 50000)
			calma += 2;
			// classificazione
			if (rabbia > calma)
			classificazione[c] = 10;
			else if (calma > rabbia)
			classificazione[c] = 0;
			else
			classificazione[c] = 5;

			rabbia = 0;
			calma = 0;*/

			somma = 0;
			if (i < SIZE - 1)
				picchio = wav.sample(i + 1);
			energiaCampione = 0;
			iteratore = 0;
			c++;
		}
		// aggiorno i valori delle singole caratteristiche nel frame corrente 
		else
		{
			SampleType campioneNormalizzato = ((wav.sample(i) - minValue) / scala) * 100;
			somma += campioneNormalizzato;
			if (campioneNormalizzato > picchio)
				picchio = campioneNormalizzato;
			energiaCampione += ((campioneNormalizzato)*(campioneNormalizzato));
			arr[iteratore] = campioneNormalizzato;
			iteratore++;
		}
		// stampa per la visualizzazione dell'avanzamento del processo
		// fa una stampa ogni 10 secondi del file audio processato
		if (i % 441000 == 0)
		{
			double pos = i, size = SIZE;
			cout << "\r[" << numeroFile << "] " << nome << " (" << SAMPLES_IN_FRAME << " samples in " << lunghezzaFrameInMs << " ms): " << (pos / size) * 100 << "%         ";
		}
	}

	/*	mi creo l'array con le somme dei valori ricavati (per fare poi la media) usando questi indici:
	0 -> discretizzatore
	1 -> media
	2 -> picchi
	3 -> energia
	4 -> frequenzeH0
	5 -> frequenzeH1
	6 -> frequenzeH2
	7 -> frequenzaFond
	8 -> posFreqFond
	9 -> pendenze */
	int sommatore[] = { sommaDisc, sommaMedia, sommaPicchi, sommaEner,
	sommaF0, sommaF1, sommaF2, sommaFreqFond, sommaPosFreqFond, sommaPend };
	// scrittura dei valori ottenuti nel file
	scrittura(LENGTH, nome, discretizzatore, media, picchi, energia, frequenzeH0, frequenzeH1, frequenzeH2,
		frequenzaFond, posFreqFond, pendenze, sommatore, fileOutput);

	cout << "\r[" << numeroFile << "] " << nome << " (" << SAMPLES_IN_FRAME << " samples in " << lunghezzaFrameInMs << " ms): 100%                    \n";

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
	ifstream risorse(argv[2]);
	ofstream fileOutput(argv[3]);
	fileOutput << "nome_file;caratteristiche" << endl << endl;
	int numeroFile = 1;
	char ch;
	string s, percorso = "";
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
			if (numeroFile == 7)
				fileOutput << endl;
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