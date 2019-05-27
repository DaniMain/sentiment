// Aquila_vs2012.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"

#include <iostream>
#include "aquila/transform/Fft.h"
#include "aquila/transform/FftFactory.h"
#include "aquila/source/generator/SineGenerator.h"
#include "aquila/source/window/BarlettWindow.h"
#include "aquila/source/window/BlackmanWindow.h"
#include "aquila/source/window/FlattopWindow.h"
#include "aquila/source/window/HammingWindow.h"
#include "aquila/source/window/HannWindow.h"
#include "aquila/source/window/RectangularWindow.h"
#include "aquila/tools/TextPlot.h"
#include <algorithm>
#include <functional>
#include <memory>

using namespace std;
using namespace Aquila;

int main()
{
	/* ATTENZIONE!!! */
	/* LENGTH non può superare il valore di 64 */
	/* altrimenti vengono sollevati errori di caricamento della memoria */
	const size_t LENGTH = 64;

	const FrequencyType sampleFreq = 2000,
		flp = 256, fhp = 512, f1 = 128, f2 = 256;
	TextPlot plot;


	// creazione di un onda casuale in un array
	SampleType arr[LENGTH];
	for (int i = 0; i<LENGTH; ++i)
	{
		arr[i] = rand() % 32;
	}

	// plot dell'onda
	plot.setTitle("Onda random creata in un array");
	plot.plot(arr, LENGTH);

	// spettro dell'onda usando la fft
	auto fft = FftFactory::getFft(LENGTH);
	SpectrumType spectrum = fft->fft(arr);
	plot.setTitle("Spettro dell'onda");
	plot.plotSpectrum(spectrum);

	SpectrumType spectrum2 = spectrum, spectrum3 = spectrum;


	// genera un filtro passa basso per lo spettro
	SpectrumType filterSpectrumLow(LENGTH);
	for (std::size_t i = 0; i < LENGTH; ++i)
	{
		if (i < (LENGTH * flp / sampleFreq))
		{
			// passband
			filterSpectrumLow[i] = 1.0;
		}
		else
		{
			// stopband
			filterSpectrumLow[i] = 0.0;
		}
	}
	plot.setTitle("Filtro passa basso per le frequenze minori di 256");
	plot.plotSpectrum(filterSpectrumLow);

	// spettro del segnale in uscita dal filtro passa basso
	// basta fare la moltiplicazione dei due spettri
	// (che è il complementare della convoluzione nel dominio del tempo)
	std::transform(
		std::begin(spectrum),
		std::end(spectrum),
		std::begin(filterSpectrumLow),
		std::begin(spectrum),
		[](ComplexType x, ComplexType y) { return x * y; }
	);
	plot.setTitle("Spettro del segnale se in uscita dal filtro passa basso");
	plot.plotSpectrum(spectrum);


	// fitro passa alto per lo spettro
	SpectrumType filterSpectrumHigh(LENGTH);
	for (std::size_t i = 0; i < LENGTH; ++i)
	{
		if (i >(LENGTH * fhp / sampleFreq))
		{
			filterSpectrumHigh[i] = 1.0;
		}
		else
		{
			filterSpectrumHigh[i] = 0.0;
		}
	}
	plot.setTitle("Filtro passa alto per le frequenze maggiori di 512");
	plot.plotSpectrum(filterSpectrumHigh);

	// spettro del segnale in uscita dal filtro passa alto
	std::transform(
		std::begin(spectrum2),
		std::end(spectrum2),
		std::begin(filterSpectrumHigh),
		std::begin(spectrum2),
		[](ComplexType x, ComplexType y) { return x * y; }
	);
	plot.setTitle("Spettro del segnale se in uscita dal filtro passa alto");
	plot.plotSpectrum(spectrum2);


	// fitro passa banda per lo spettro
	SpectrumType filterSpectrumBand(LENGTH);
	for (std::size_t i = 0; i < LENGTH; ++i)
	{
		if (i >(LENGTH * f1 / sampleFreq) && i < (LENGTH * f2 / sampleFreq))
		{
			filterSpectrumBand[i] = 1.0;
		}
		else
		{
			filterSpectrumBand[i] = 0.0;
		}
	}
	plot.setTitle("Filtro passa banda per le frequenze comprese tra 128 e 256");
	plot.plotSpectrum(filterSpectrumBand);

	// spettro del segnale in uscita dal filtro passa banda
	std::transform(
		std::begin(spectrum3),
		std::end(spectrum3),
		std::begin(filterSpectrumBand),
		std::begin(spectrum3),
		[](ComplexType x, ComplexType y) { return x * y; }
	);
	plot.setTitle("Spettro del segnale se in uscita dal filtro passa banda");
	plot.plotSpectrum(spectrum3);


	// Inverse FFT per lo spettro dopo il filtro passa basso
	double ifft[LENGTH];
	fft->ifft(spectrum, ifft);
	plot.setTitle("Segnale dell'onda dopo il filtro passa basso");
	plot.plot(ifft, LENGTH);


	// Inverse FFT per lo spettro dopo il filtro passa alto
	double ifft2[LENGTH];
	fft->ifft(spectrum2, ifft2);
	plot.setTitle("Segnale dell'onda dopo il filtro passa alto");
	plot.plot(ifft2, LENGTH);


	// Inverse FFT per lo spettro dopo il filtro passa banda
	double ifft3[LENGTH];
	fft->ifft(spectrum3, ifft3);
	plot.setTitle("Segnale dell'onda dopo il filtro passa banda");
	plot.plot(ifft3, LENGTH);



	// altra onda creata in un array
	SampleType onda[LENGTH];
	for (int i = 0; i<LENGTH; ++i)
	{
		onda[i] = i % 10;
	}
	plot.setTitle("Altra onda creata in un array ");
	plot.plot(onda, LENGTH);

	// spettro dell'onda
	auto fft2 = FftFactory::getFft(LENGTH);
	SpectrumType spettro = fft->fft(onda);
	plot.setTitle("Spettro dell'onda");
	plot.plotSpectrum(spettro);

	// ifft dell'onda
	double onda_from_spettro[LENGTH];
	fft->ifft(spettro, onda_from_spettro);
	plot.setTitle("Onda ricreata dallo spettro");
	plot.plot(onda_from_spettro, LENGTH);

	// onda filtrata tramite il filtro passa basso di prima
	std::transform(
		std::begin(spettro),
		std::end(spettro),
		std::begin(filterSpectrumLow),
		std::begin(spettro),
		[](ComplexType x, ComplexType y) { return x * y; }
	);
	plot.setTitle("Spettro dell'onda in uscita dal filtro passa basso precedente");
	plot.plotSpectrum(spettro);

	// ifft dello spettro ottenuto
	double onda_from_spettro_filtrato[LENGTH];
	fft->ifft(spettro, onda_from_spettro_filtrato);
	plot.setTitle("Onda creata dallo spettro filtrato");
	plot.plot(onda_from_spettro_filtrato, LENGTH);



	// sine generator
	SineGenerator sine = SineGenerator(64);
	sine.setAmplitude(32).setFrequency(125).generate(64);
	plot.setTitle("Sine generator");
	plot.plot(sine);

	// energia del segnale creato
	double energia = energy(sine);
	cout << "L'energia del segnale creato e': " << energia << endl;

	// spettro di sine
	auto fft_sine = FftFactory::getFft(64);
	SpectrumType spettro_di_sine = fft_sine->fft(sine.toArray());
	plot.setTitle("Spettro di sine");
	plot.plotSpectrum(spettro_di_sine);


	system("pause");
	return 0;
}