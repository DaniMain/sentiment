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
	int *picchi, int *energia, int *frequenzeH0, int *frequenzeH1, int *frequenzeH2,
	int *frequenzaFond, int *posFreqFond, int *pendenze, char **classificazione, int *classNum, ofstream & fileOutput) {
	fileOutput << nome << ";discretizzatore;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << discretizzatore[i] << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";media;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << media[i] << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";picchi;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << picchi[i] << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";energia;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << energia[i] << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";frequenzeH0;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << frequenzeH0[i] << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";frequenzeH1;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << frequenzeH1[i] << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";frequenzeH2;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << frequenzeH2[i] << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";frequenzaFond;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << frequenzaFond[i] << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";posFreqFond;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << posFreqFond[i] << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";pendenze;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << pendenze[i] << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";classificazione;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << classificazione[i] << ";";
	}
	fileOutput << endl;

	fileOutput << nome << ";classNum;";
	for (int i = 0; i <= LENGTH; i++)
	{
		fileOutput << classNum[i] << ";";
	}
	fileOutput << endl;
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
	int *sommaCoeff = new int[LENGTH];
	int *mediaCoeff = new int[LENGTH];
	int *maxCoeff = new int[LENGTH];
	int *minCoeff = new int[LENGTH];
	int *energia = new int[LENGTH];
	/*SampleType minValue = wav.sample(0);
	for (auto it = wav.begin(); it != wav.end(); ++it)
	{
	if (*it < minValue)
	minValue = *it;
	}*/
	SampleType *arr;
	arr = new SampleType[SAMPLES_IN_FRAME];
	int iteratore = 0;
	for (size_t i = 0; i < SIZE; i++)
	{
		// se sto a un multiplo di lunghezzaFrameInMs aggiorno gli array con i valori ottenuti (vedi else)
		if ((i % SAMPLES_IN_FRAME == 0 || i == SIZE - 1) && i != 0)
		{
			SampleType posCorrente = wav.sample(i);
			arr[iteratore] = posCorrente;
			SignalSource segnale(arr, LENGTH, 2);
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

			/* analisi dei risultati ottenuti in base ai criteri illustrati nel file euristica.docx */
			// discretizzatore
			/*if (discretizzatore[c] >= 1000000)
				rabbia += 1;
			else if (discretizzatore[c] >= 2000000)
				rabbia += 2;
			else if (discretizzatore[c] >= 5000000)
				rabbia += 3;
			else if (discretizzatore[c] >= 10000000)
				rabbia += 4;
			else if (discretizzatore[c] < 200000)
				calma++;
			else if (discretizzatore[c] < 100000)
				calma += 2;
			// picchi
			if (picchi[c] >= 17000)
				rabbia++;
			else if (picchi[c] >= 20000)
				rabbia += 2;
			else if (picchi[c] >= 30000)
				rabbia += 3;
			else if (picchi[c] < 17000)
				calma++;
			else if (picchi[c] < 10000)
				calma += 2;
			else if (picchi[c] < 5000)
				calma += 3;
			// energia
			if (energia[c] >= 60000)
				rabbia++;
			else if (energia[c] >= 100000)
				rabbia += 2;
			else if (energia[c] >= 200000)
				rabbia += 3;
			else if (energia[c] >= 500000)
				rabbia += 4;
			else if (energia[c] < 60000)
				calma++;
			else if (energia[c] < 20000)
				calma += 2;
			else if (energia[c] < 5000)
				calma += 3;
			// frequenzeH0
			if (frequenzeH0[c] >= 150000)
				rabbia++;
			else if (frequenzeH0[c] >= 300000)
				rabbia += 2;
			else if (frequenzeH0[c] >= 500000)
				rabbia += 3;
			else if (frequenzeH0[c] < 50000)
				calma += 2;
			// frequenzeH1
			if (frequenzeH1[c] >= 100000)
				rabbia++;
			else if (frequenzeH1[c] >= 200000)
				rabbia += 2;
			else if (frequenzeH1[c] >= 300000)
				rabbia += 3;
			else if (frequenzeH1[c] < 100000)
				calma++;
			else if (frequenzeH1[c] < 30000)
				calma += 2;
			// frequenzeH2
			if (frequenzeH2[c] >= 50000)
				rabbia++;
			else if (frequenzeH2[c] >= 100000)
				rabbia += 2;
			else if (frequenzeH2[c] >= 200000)
				rabbia += 3;
			else if (frequenzeH2[c] < 50000)
				calma++;
			else if (frequenzeH2[c] < 30000)
				calma += 2;
			// frequenzaFond
			if (frequenzaFond[c] >= 100000)
				rabbia++;
			else if (frequenzaFond[c] >= 250000)
				rabbia += 2;
			else if (frequenzaFond[c] >= 330000)
				rabbia += 3;
			else if (frequenzaFond[c] < 100000)
				calma++;
			else if (frequenzaFond[c] < 50000)
				calma += 2;
			// pendenze
			if (pendenze[c] >= 100000)
				rabbia++;
			else if (pendenze[c] >= 250000)
				rabbia += 2;
			else if (pendenze[c] >= 330000)
				rabbia += 3;
			else if (pendenze[c] < 100000)
				calma++;
			else if (pendenze[c] < 50000)
				calma += 2;
			// classificazione
			if (rabbia > calma) {
				classNum[c] = 10;
				emozione = "arrabbiato";
			}
			else if (calma > rabbia) {
				classNum[c] = 0;
				emozione = "calmo";
			}
			else {
				classNum[c] = 5;
				emozione = "neutro";
			}
			classificazione[c] = emozione;

			rabbia = 0;
			calma = 0;
			*/
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
			SampleType posCorrente = wav.sample(i);
			somma += posCorrente;
			if (posCorrente > picchio)
				picchio = posCorrente;
			energiaCampione += ((posCorrente)*(posCorrente));
			arr[iteratore] = posCorrente;
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

	// scrittura dei valori ottenuti nel file
	scrittura(LENGTH, nome, discretizzatore, media, picchi, energia, frequenzeH0, frequenzeH1, frequenzeH2,
		frequenzaFond, posFreqFond, pendenze, classificazione, classNum, fileOutput);

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
	int numeroFile = 1;
	ifstream risorse(argv[2]);
	ofstream fileOutput(argv[3]);
	fileOutput << "nome_file;caratteristiche" << endl;
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