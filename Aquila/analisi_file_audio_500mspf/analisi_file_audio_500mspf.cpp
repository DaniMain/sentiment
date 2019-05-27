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

int main(int argc, char * argv[])
{
	if (argc < 3)
	{
		cout << "parametri insufficienti [" << argc << "]" << endl;
		return 1;
	}

	const int lunghezzaFrameInMs = 500;
	ifstream risorse(argv[1]);
	ofstream fileOutput(argv[2]);
	fileOutput << "nome_file;caratteristiche" << endl;
	char ch;
	string s, tot = "";
	while (!risorse.eof())
	{
		risorse.get(ch);
		// estraggo la stringa con il percorso
		if (ch != '\n')
		{
			s = ch;
			tot.append(s);
		}
		// una volta ottenuto il percorso analizzo il file
		else
		{
			WaveFile wav(tot);
			string nome = wav.getFilename().erase(0, 64);
			system("cls");
			cout << nome;
			double SIZE = wav.getSamplesCount();
			double audioLength = wav.getAudioLength();
			size_t SAMPLES_IN_500_MS = (SIZE / audioLength) * lunghezzaFrameInMs;
			cout << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)\n";
			int c = 0;
			size_t zero = 0, quattro = 4, sei = 6;
			int LENGTH = audioLength / lunghezzaFrameInMs;
			SampleType *discretizzatore = new SampleType[LENGTH];
			SampleType *media = new SampleType[LENGTH];
			SampleType *picchi = new SampleType[LENGTH];
			SpectrumType spettro;
			double *frequenzeH0 = new double[LENGTH];
			double *frequenzeH4 = new double[LENGTH];
			double *frequenzeH6 = new double[LENGTH];
			double *energia = new double[LENGTH];
			int *posFreqFond = new int[LENGTH];
			double *pendenze = new double[LENGTH];
			SampleType minValue = wav.sample(0);
			for (auto it = wav.begin(); it != wav.end(); ++it)
			{
				if (*it < minValue)
					minValue = *it;
			}

			SampleType somma = 0;
			SampleType picchio = minValue;
			double energiaCampione = 0;
			SampleType *arr;
			arr = new SampleType[SAMPLES_IN_500_MS];
			int iteratore = 0;
			for (size_t i = 0; i < SIZE; i++)
			{
				// se sto a un multiplo di 500ms aggiorno gli array con i valori ottenuti (vedi else)
				if ((i % SAMPLES_IN_500_MS == 0 || i == SIZE - 1) && i != 0)
				{
					SampleType posCorrente = wav.sample(i);
					somma += posCorrente;
					if (posCorrente > picchio)
						picchio = posCorrente;
					energiaCampione += ((posCorrente)*(posCorrente));
					arr[iteratore] = posCorrente;
					discretizzatore[c] = somma;
					media[c] = (somma / SAMPLES_IN_500_MS);
					somma = 0;
					picchi[c] = picchio;
					picchio = minValue;
					energia[c] = energiaCampione;
					energiaCampione = 0;
					auto fft = FftFactory::getFft(64);
					SpectrumType spettro = fft->fft(arr);
					frequenzeH0[c] = abs(spettro.at(0));
					frequenzeH4[c] = abs(spettro.at(4));
					frequenzeH6[c] = abs(spettro.at(6));
					int pos = 0;
					double freqFond = abs(spettro.at(0));
					for (int j = 1; j < 64; j++)
					{
						if (abs(spettro.at(j)) > freqFond) {
							pos = j;
							freqFond = abs(spettro.at(j));
						}
					}
					posFreqFond[c] = pos;
					// il valore della pendenza è dato come la differenza tra il valore della
					// frequenza fondamentale e il valore della frequenza precedente o successiva
					if (pos == 0)
						pendenze[c] = (abs(spettro.at(pos)) - abs(spettro.at(pos + 1)));
					else if (pos == 63)
						pendenze[c] = (abs(spettro.at(pos)) - abs(spettro.at(pos - 1)));
					else
						pendenze[c] = __max((abs(spettro.at(pos)) - abs(spettro.at(pos - 1))), (abs(spettro.at(pos)) - abs(spettro.at(pos + 1))));
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
				if (i % 441000 == 0)
				{
					system("cls");
					double pos = i, size = SIZE;
					cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << (pos / size) * 100 << "%";
				}
			}

			system("cls");
			cout << nome << ": 100%";

			/* scrittura dei valori ottenuti nel file */

			fileOutput << nome << ";discretizzatore;";
			for (int i = 0; i <= LENGTH; i++)
			{
				fileOutput << discretizzatore[i] << ";";
			}
			fileOutput << endl;
			system("cls");
			cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% *";

			fileOutput << nome << ";media;";
			for (int i = 0; i <= LENGTH; i++)
			{
				fileOutput << media[i] << ";";
			}
			fileOutput << endl;
			system("cls");
			cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% **";

			fileOutput << nome << ";picchi;";
			for (int i = 0; i <= LENGTH; i++)
			{
				fileOutput << picchi[i] << ";";
			}
			fileOutput << endl;
			system("cls");
			cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% ***";

			fileOutput << nome << ";energia;";
			for (int i = 0; i <= LENGTH; i++)
			{
				fileOutput << energia[i] << ";";
			}
			fileOutput << endl;
			system("cls");
			cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% ****";

			fileOutput << nome << ";frequenzeH0;";
			for (int i = 0; i <= LENGTH; i++)
			{
				fileOutput << frequenzeH0[i] << ";";
			}
			fileOutput << endl;
			system("cls");
			cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% *****";

			fileOutput << nome << ";frequenzeH4;";
			for (int i = 0; i <= LENGTH; i++)
			{
				fileOutput << frequenzeH4[i] << ";";
			}
			fileOutput << endl;
			system("cls");
			cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% ******";

			fileOutput << nome << ";frequenzeH6;";
			for (int i = 0; i <= LENGTH; i++)
			{
				fileOutput << frequenzeH6[i] << ";";
			}
			fileOutput << endl;
			system("cls");
			cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% *******";

			fileOutput << nome << ";posFreqFond;";
			for (int i = 0; i <= LENGTH; i++)
			{
				fileOutput << posFreqFond[i] << ";";
			}
			fileOutput << endl;
			system("cls");
			cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% ********";

			fileOutput << nome << ";pendenze;";
			for (int i = 0; i <= LENGTH; i++)
			{
				fileOutput << pendenze[i] << ";";
			}
			fileOutput << endl;
			system("cls");
			cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% *********";
			tot = "";
		}
	}

	// l'ultima stringa ottenuta dal file delle risorse se non vuota contiene l'ultimo percorso
	if (!tot.empty())
	{
		int size = tot.size() - 1;
		tot.erase(size, size - 1);
		WaveFile wav(tot);
		string nome = wav.getFilename().erase(0, 64);
		system("cls");
		cout << nome;
		double SIZE = wav.getSamplesCount();
		double audioLength = (size_t)wav.getAudioLength();
		size_t SAMPLES_IN_500_MS = (SIZE / audioLength) * lunghezzaFrameInMs;
		cout << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)\n";
		int c = 0;
		size_t zero = 0, quattro = 4, sei = 6;
		int LENGTH = audioLength / lunghezzaFrameInMs;
		SampleType *discretizzatore = new SampleType[LENGTH];
		SampleType *media = new SampleType[LENGTH];
		SampleType *picchi = new SampleType[LENGTH];
		SpectrumType spettro;
		double *frequenzeH0 = new double[LENGTH];
		double *frequenzeH4 = new double[LENGTH];
		double *frequenzeH6 = new double[LENGTH];
		double *energia = new double[LENGTH];
		int *posFreqFond = new int[LENGTH];
		double *pendenze = new double[LENGTH];
		SampleType minValue = wav.sample(0);
		for (auto it = wav.begin(); it != wav.end(); ++it)
		{
			if (*it < minValue)
				minValue = *it;
		}

		SampleType somma = 0;
		SampleType picchio = minValue;
		double energiaCampione = 0;
		SampleType *arr;
		arr = new SampleType[SAMPLES_IN_500_MS];
		int iteratore = 0;
		for (size_t i = 0; i < SIZE; i++)
		{
			// se sto a un multiplo di 500ms aggiorno gli array con i valori ottenuti (vedi else)
			if ((i % SAMPLES_IN_500_MS == 0 || i == SIZE - 1) && i != 0)
			{
				SampleType posCorrente = wav.sample(i);
				somma += posCorrente;
				if (posCorrente > picchio)
					picchio = posCorrente;
				energiaCampione += ((posCorrente)*(posCorrente));
				arr[iteratore] = posCorrente;
				discretizzatore[c] = somma;
				media[c] = (somma / SAMPLES_IN_500_MS);
				somma = 0;
				picchi[c] = picchio;
				picchio = minValue;
				energia[c] = energiaCampione;
				energiaCampione = 0;
				auto fft = FftFactory::getFft(64);
				SpectrumType spettro = fft->fft(arr);
				frequenzeH0[c] = abs(spettro.at(0));
				frequenzeH4[c] = abs(spettro.at(4));
				frequenzeH6[c] = abs(spettro.at(6));
				int pos = 0;
				double freqFond = abs(spettro.at(0));
				for (int j = 1; j < 64; j++)
				{
					if (abs(spettro.at(j)) > freqFond) {
						pos = j;
						freqFond = abs(spettro.at(j));
					}
				}
				posFreqFond[c] = pos;
				// il valore della pendenza è dato come la differenza tra il valore della
				// frequenza fondamentale e il valore della frequenza precedente o successiva
				if (pos == 0)
					pendenze[c] = (abs(spettro.at(pos)) - abs(spettro.at(pos + 1)));
				else if (pos == 63)
					pendenze[c] = (abs(spettro.at(pos)) - abs(spettro.at(pos - 1)));
				else
					pendenze[c] = __max((abs(spettro.at(pos)) - abs(spettro.at(pos - 1))), (abs(spettro.at(pos)) - abs(spettro.at(pos + 1))));
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
				system("cls");
				double pos = i, size = SIZE;
				cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << (pos / size) * 100 << "%";
			}
		}

		system("cls");
		cout << nome << ": 100%";

		/* scrittura dei valori ottenuti nel file */

		fileOutput << nome << ";discretizzatore;";
		for (int i = 0; i <= LENGTH; i++)
		{
			fileOutput << discretizzatore[i] << ";";
		}
		fileOutput << endl;
		system("cls");
		cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% *";

		fileOutput << nome << ";media;";
		for (int i = 0; i <= LENGTH; i++)
		{
			fileOutput << media[i] << ";";
		}
		fileOutput << endl;
		system("cls");
		cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% **";

		fileOutput << nome << ";picchi;";
		for (int i = 0; i <= LENGTH; i++)
		{
			fileOutput << picchi[i] << ";";
		}
		fileOutput << endl;
		system("cls");
		cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% ***";

		fileOutput << nome << ";energia;";
		for (int i = 0; i <= LENGTH; i++)
		{
			fileOutput << energia[i] << ";";
		}
		fileOutput << endl;
		system("cls");
		cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% ****";

		fileOutput << nome << ";frequenzeH0;";
		for (int i = 0; i <= LENGTH; i++)
		{
			fileOutput << frequenzeH0[i] << ";";
		}
		fileOutput << endl;
		system("cls");
		cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% *****";

		fileOutput << nome << ";frequenzeH4;";
		for (int i = 0; i <= LENGTH; i++)
		{
			fileOutput << frequenzeH4[i] << ";";
		}
		fileOutput << endl;
		system("cls");
		cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% ******";

		fileOutput << nome << ";frequenzeH6;";
		for (int i = 0; i <= LENGTH; i++)
		{
			fileOutput << frequenzeH6[i] << ";";
		}
		fileOutput << endl;
		system("cls");
		cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% *******";

		fileOutput << nome << ";posFreqFond;";
		for (int i = 0; i <= LENGTH; i++)
		{
			fileOutput << posFreqFond[i] << ";";
		}
		fileOutput << endl;
		system("cls");
		cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% ********";

		fileOutput << nome << ";pendenze;";
		for (int i = 0; i <= LENGTH; i++)
		{
			fileOutput << pendenze[i] << ";";
		}
		fileOutput << endl;
		system("cls");
		cout << nome << " (" << SAMPLES_IN_500_MS << " samples in 0.5 s)" << "\n" << "100% *********";
	}

	risorse.close();
	fileOutput.close();

	return 0;
}