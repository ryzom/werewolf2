#ifndef __GPUORENNAYARFP_H__
#define __GPUORENNAYARFP_H__

#include <GpuManager/GpuProgram.h>
#include <PropertyManager/PropertyManager.h>
#include <PropertyManager/CBasicProperty.h>
#include <PropertyManager/CFunctionSource.h>

class GpuOrenNayarFP : public GpuProgram {

public:

	GpuOrenNayarFP(std::string file) : GpuProgram(file) {};

	void activate() {
		GpuProgram::activate();
		if(!PropertyManager::instance().getProperty("USER", "USER.OrenNayarCoeffs2")) {
			PropertyManager::instance().registerProperty("USER", new CBasicProperty<float>(new CFunctionSource<float>(&GpuOrenNayarFP::calcOrenNayar, 2), "USER.OrenNayarCoeffs2"));
		}
	};

	static float* calcOrenNayar() {
		static float coeffs[2];
		float* icoeffs = (dynamic_cast<CBasicProperty<float>*>(PropertyManager::instance().getProperty("USER", "USER.OrenNayarCoeffs")))->getValue();
		float sigma = icoeffs[1];
		coeffs[0] = 1 - (.5 * sigma * sigma) / (sigma * sigma + .33);
		coeffs[1] = (.45 * sigma * sigma) / (sigma * sigma + .09);
		return &coeffs[0];
	}

	static GpuProgram* create(std::string file) {
		return new GpuOrenNayarFP(file);
	}
};

#endif // __GPUORENANAYARFP_H__