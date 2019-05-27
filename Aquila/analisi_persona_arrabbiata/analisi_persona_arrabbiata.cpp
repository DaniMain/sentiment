// analisi_persona_arrabbiata.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"
#include "aquila/global.h"
#include "aquila/source/WaveFile.h"
#include "aquila/tools/TextPlot.h"
#include "aquila/transform/FftFactory.h"
#include "aquila/transform.h"
#include <cstddef>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <functional>
#include <memory>

using namespace Aquila;
using namespace std;

int main(int argc, char *argv[])
{
	/* 
	C:\Users\User\Documents\sentiment_per_tirocinio\sentiment\audio\Persona_abbastanza_arrabbiata_5_sec.wav
	C:\Users\User\Documents\sentiment_per_tirocinio\sentiment\audio\Ragazzo_si_incazza_e_rompe_il_computer.wav
	C:\Users\User\Documents\sentiment_per_tirocinio\sentiment\audio\Super_Angry_Australian.wav
	C:\Users\User\Desktop\¡DIEM!\CELL\MUSIC\VOICE\prova.wav
	*/

	const size_t SIZE = 64;
	TextPlot plot;

	WaveFile wav(argv[1]);
	cout << "Filename: " << wav.getFilename();
	cout << "\nLength: " << wav.getAudioLength() << " ms";
	cout << "\nSample frequency: " << wav.getSampleFrequency() << " Hz";
	cout << "\nChannels: " << wav.getChannelsNum();
	cout << "\nByte rate: " << wav.getBytesPerSec() / 1024 << " kB/s";
	cout << "\nBits per sample: " << wav.getBitsPerSample() << "b\n";

	/*cout << "Loaded file: " << wav.getFilename()
		<< " (" << wav.getBitsPerSample() << "b)" << endl;*/
	SampleType maxValue = wav.sample(0), minValue = wav.sample(0), average = 0;

	cout << endl;

	// samples count
	cout << "There are " << wav.getSamplesCount() << " samples\n";

	// samples visualization
	/*std::cout << "Samples values:\n";
	for (size_t i = 0; i < wav.getSamplesCount(); i++)
	{
		cout << wav.sample(i) << " ";
	}
	cout << std::endl;*/

	// simple index-based iteration
	for (size_t i = 0; i < wav.getSamplesCount(); ++i)
	{
		if (wav.sample(i) > maxValue)
			maxValue = wav.sample(i);
	}
	cout << "Maximum sample value: " << maxValue << endl;

	// iterator usage
	for (auto it = wav.begin(); it != wav.end(); ++it)
	{
		if (*it < minValue)
			minValue = *it;
	}
	cout << "Minimum sample value: " << minValue << endl;

	// range-based for loop
	for (auto sample : wav)
	{
		average += sample;
	}
	average /= wav.getSamplesCount();
	cout << "Average: " << average << endl;

	// STL algorithms work too, so the previous examples could be rewritten
	// using max/min_element.
	int limit = 0;
	int aboveLimit = count_if(
		wav.begin(),
		wav.end(),
		[limit](SampleType sample) {
		return sample > limit;
	}
	);
	cout << "There are " << aboveLimit << " samples greater than "
		<< limit << endl << endl;

	/*TextPlot plot("wav plot");
	plot.plot(wav);*/

	double energia = energy(wav);
	cout << "L'energia del segnale del file audio e': " << energia << endl;

	// calcolo dello spettro
	auto fft = FftFactory::getFft(SIZE);
	SpectrumType spettro = fft->fft(wav.toArray());
	plot.setTitle("Spettro del file audio:");
	plot.plotSpectrum(spettro);
	cout << endl;

	// stampa i valori delle frequenze
	cout << "Valore delle frequenze:\n";
	for (int i = 0; i < SIZE; ++i)
	{
		cout << spettro[i] << " ";
	}
	cout << endl << endl;

	// stampa dei valori dei picchi dello spettro
	/*transform(
		begin(spettro),
		end(spettro),
		begin(spettro),
		begin(spettro),
		[](ComplexType x, ComplexType y) { return x; }
	);*/

		
	system("pause");
    return 0;
}

