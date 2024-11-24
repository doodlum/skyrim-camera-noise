#pragma once

#include "CameraNoise_API.h"

class CameraNoiseInterfaceImpl : public CameraNoise_API::CameraNoiseInterface
{
public:
	static CameraNoiseInterfaceImpl* GetSingleton()
	{
		static CameraNoiseInterfaceImpl singleton;
		return &singleton;
	}

	virtual bool LoadIni(std::string filepath) override;
	virtual bool UnloadIni(std::string filepath) override;
	virtual void ResetIni() override;

protected:
	CameraNoiseInterfaceImpl() = default;
	CameraNoiseInterfaceImpl(const CameraNoiseInterfaceImpl&) = delete;
	CameraNoiseInterfaceImpl(CameraNoiseInterfaceImpl&&) = delete;
	virtual ~CameraNoiseInterfaceImpl() = default;

	auto operator=(const CameraNoiseInterfaceImpl&) -> CameraNoiseInterfaceImpl& = delete;
	auto operator=(CameraNoiseInterfaceImpl&&) -> CameraNoiseInterfaceImpl& = delete;
};
