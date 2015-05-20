% User Selectable Parameters:

samplingFrequencyHz = 48000;
filterCentreFrequencyHz = 12000;
filterBandwidthHz = 200;
filterOrder = 2;
onAmplitude = 1;
offAmplitude = 0.5;
offShortDurationSeconds = 0.1;
offLongDurationSeconds = 0.2;

offShort = offAmplitude * ones([1, samplingFrequencyHz * offShortDurationSeconds]);
offLong = offAmplitude * ones([1, samplingFrequencyHz * offLongDurationSeconds]);

modulation = onAmplitude * ones([1, samplingFrequencyHz]);
modulation(100:100 + length(offShort) - 1) = offShort;
modulation(9000:9000 + length(offLong) - 1) = offLong;


% Calculated Parameters:

samplingInterval = 1 / samplingFrequencyHz;
samplingNyquistHz = samplingFrequencyHz / 2;
samplingTimes = linspace(0, 1 - samplingInterval, samplingFrequencyHz);

filterLowerFrequencyHz = filterCentreFrequencyHz - filterBandwidthHz / 2;
filterUpperFrequencyHz = filterCentreFrequencyHz + filterBandwidthHz / 2;


% Undersampled Graph:

msfFrequencyHz = 60000;
msfSamples = modulation .* sin(2 * pi() * msfFrequencyHz * samplingTimes);

setenv('GNUTERM', 'wxt');
figure('name', 'MSF Samples');
plot([1:length(msfSamples)], msfSamples);
title('MSF Samples');
xlabel('Sample Number');
ylabel('Amplitude');


% Filtered Samples Graph:

filterNormalisedPassband = [(filterLowerFrequencyHz / samplingNyquistHz) (filterUpperFrequencyHz / samplingNyquistHz)];
[b, a] = butter(filterOrder, filterNormalisedPassband);
filtered = filter(b, a, msfSamples);

figure('name', 'Filtered MSF Samples');
plot([1:length(filtered)], filtered);
title('Filtered MSF Samples');
xlabel('Sample Number');
ylabel('Amplitude');


% FFT Graph:

numberOfFftBins = 2 ^ nextpow2(length(filtered));
fftBins = fft(filtered, numberOfFftBins) / length(filtered);
fftFrequencies = samplingNyquistHz * linspace(0, 1, numberOfFftBins / 2 + 1);

figure('name', 'FFT of Filtered MSF Samples');
plot(fftFrequencies, 2 * abs(fftBins(1:numberOfFftBins / 2 + 1)));
title('FFT of Filtered MSF Samples');
xlabel('Frequency (Hz)');
ylabel('Absolute Magnitude');


% Block parameters:

blockTimeSeconds = 0.01;
blockSize = floor(blockTimeSeconds * samplingFrequencyHz);


% Blackman Window:

alpha = 0.16;
a0 = (1 - alpha) / 2;
a1 = 0.5;
a2 = alpha / 2;

for n = 0:blockSize - 1
	window(n + 1) = a0 - a1 * cos(2 * pi() * n / blockSize) + a2 * cos(4 * pi() * n / blockSize);
end;

figure('name', 'Window');
plot([0:length(window) - 1], window);
title('Window');
xlabel('Sample Number');
ylabel('Coefficient');


% Goertzel Output:

normalisedFrequencyHz = msfFrequencyHz / samplingFrequencyHz;
coefficient = 2 * cos(2 * pi() * normalisedFrequencyHz / blockSize);

for blockNumber = 1:(1 / blockTimeSeconds)
	blockStart = (blockNumber - 1) * blockSize + 1;
	blockEnd = blockNumber * blockSize;
	block = msfSamples(blockStart:blockEnd);

	d1 = 0;
	d2 = 0;
	for n = 1:blockSize
    		y = window(n) * block(n) + coefficient * d1 - d2;
    		d2 = d1;
    		d1 = y;
	end;

	goertzelPower(blockNumber) = d1 * d1 + d2 * d2 - coefficient * d1 * d2;
end;

xAxis = [0:length(goertzelPower) - 1] * blockTimeSeconds;
figure('name', 'Goertzel Output');
plot(xAxis, goertzelPower);
title('Goertzel Output');
xlabel('Time (s)');
ylabel('Power');

pause;
