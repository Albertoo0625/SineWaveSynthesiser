#include<iostream>
#include<vector>
#include"olcNoiseMaker.h"
#include<atomic>
#include"Envelope.h"

atomic<double> dFrequencyOutput = 0.0;

double w(double dHertz)
{
	return dHertz * 2 * PI;
}
double osc(double dHertz,double dTime,int nType)
{
	switch (nType)
	{
	case 0: // sine wave
		
		return sin(w(dHertz) * dTime);

	case 1: // square wave
		return sin(w(dHertz) * dTime) > 0.0 ? 1.0 : -1.0;

	case 2: // triangle wave
		return asin(sin(w(dHertz) * dTime)) * 2 / PI;

	case 3: //saw wave analogue
	{
		double dOutput = 0.0;

		for (double n = 1.0;n < 10.0;n++) 
		{
			dOutput += (sin(n * w(dHertz) * dTime)) / n;
		}
		return dOutput * (2.0 / PI);
	}

	case 4: // saw wave fmod
		return (2.0 / PI) * (dHertz * PI * fmod(dTime, 1.0 / dHertz) - (PI / 2.0));

	case 5: 
		return 2.0 * ((double)rand() / (double)RAND_MAX) - 1.0;

	default:
		return 0.0;
	}
}



sEnvelopeADSR envelope;

double MakeNoise(double dTime)
{
	double dOutput = envelope.GetAmplitude(dTime) * osc(dFrequencyOutput, dTime, 3);
	return dOutput;
}

int main()
{
	vector<wstring>devices=olcNoiseMaker<short>::Enumerate();

	for (auto d : devices) wcout << "Found Output Device: " << d << endl;
	wcout << "Using Device: " << devices[0] << endl;

	// Display a keyboard
	wcout << endl <<
		"|   |   |   |   |   | |   |   |   |   | |   | |   |   |   |" << endl <<
		"|   | S |   |   | F | | G |   |   | J | | K | | L |   |   |" << endl <<
		"|   |___|   |   |___| |___|   |   |___| |___| |___|   |   |__" << endl <<
		"|     |     |     |     |     |     |     |     |     |     |" << endl <<
		"|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |" << endl <<
		"|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|" << endl << endl;

	// Create sound machine!!
	olcNoiseMaker<short> sound(devices[0], 44100, 1, 8, 512);

	sound.SetUserFunction(MakeNoise);

	double dOctaveBaseFrequency = 110.0;
	double d12thRootOf2 = pow(2.0, 1.0 / 12.0);



	while(true)
	{
		bool keyPressed = false;
		for(int k = 0;k < 16;k++)
		{
			if (GetAsyncKeyState((unsigned char)("ZSXCFVGBNJMK\xbcL\xbe\xbf"[k])) & 0x8000)
			{
				dFrequencyOutput = dOctaveBaseFrequency * pow(d12thRootOf2, k);
				envelope.NoteOn(sound.GetTime());
				keyPressed=true;
	
			}		
		}
		if (!keyPressed)
		{	                
			envelope.NoteOff(sound.GetTime());
		}
	}
	return 0;
}