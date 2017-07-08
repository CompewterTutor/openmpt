/*
 * I3DL2Reverb.h
 * -------------
 * Purpose: Implementation of the DMO I3DL2Reverb DSP (for non-Windows platforms)
 * Notes  : (currently none)
 * Authors: OpenMPT Devs
 * The OpenMPT source code is released under the BSD license. Read LICENSE for more details.
 */


#pragma once

#ifndef NO_PLUGINS

#include "../PlugInterface.h"

OPENMPT_NAMESPACE_BEGIN

namespace DMO
{

//===================================
class I3DL2Reverb : public IMixPlugin
//===================================
{
protected:
	enum Parameters
	{
		kI3DL2ReverbRoom = 0,
		kI3DL2ReverbRoomHF,
		kI3DL2ReverbRoomRolloffFactor,	// Doesn't actually do anything :)
		kI3DL2ReverbDecayTime,
		kI3DL2ReverbDecayHFRatio,
		kI3DL2ReverbReflections,
		kI3DL2ReverbReflectionsDelay,
		kI3DL2ReverbReverb,
		kI3DL2ReverbReverbDelay,
		kI3DL2ReverbDiffusion,
		kI3DL2ReverbDensity,
		kI3DL2ReverbHFReference,
		kI3DL2ReverbQuality,
		kI3DL2ReverbNumParameters
	};

	enum QualityFlags
	{
		kMoreDelayLines = 0x01,
		kFullSampleRate = 0x02,
	};

	class DelayLine : private std::vector<float>
	{
		int32 m_length;
		int32 m_position;
		int32 m_delayPosition;

	public:
		void Init(int32 ms, int32 padding, uint32 sampleRate, int32 delayTap = 0);
		void SetDelayTap(int32 delayTap);
		void Advance();
		void Set(float value);
		float Get(int32 offset) const;
		float Get() const;
	};

	float m_param[kI3DL2ReverbNumParameters];

	// Calculated parameters
	uint32 m_quality;
	float m_effectiveSampleRate;
	float m_diffusion;
	float m_roomFilter;
	float m_ERLevel;
	float m_ReverbLevelL;
	float m_ReverbLevelR;

	int32 m_delayTaps[15];	// 6*L + 6*R + LR + Early L + Early R
	int32 m_earlyTaps[2][6];
	float m_delayCoeffs[13][2];

	// State
	DelayLine m_delayLines[19];
	float m_filterHist[19];

	// Remaining frame for downsampled reverb
	float m_prevL;
	float m_prevR;
	bool m_remain;

	bool m_ok, m_recalcParams;

public:
	static IMixPlugin* Create(VSTPluginLib &factory, CSoundFile &sndFile, SNDMIXPLUGIN *mixStruct);
	I3DL2Reverb(VSTPluginLib &factory, CSoundFile &sndFile, SNDMIXPLUGIN *mixStruct);

	virtual void Release() { delete this; }
	virtual int32 GetUID() const { return 0xEF985E71; }
	virtual int32 GetVersion() const { return 0; }
	virtual void Idle() { }
	virtual uint32 GetLatency() const { return 0; }

	virtual void Process(float *pOutL, float *pOutR, uint32 numFrames);

	virtual float RenderSilence(uint32) { return 0.0f; }

	virtual int32 GetNumPrograms() const { return 0; }
	virtual int32 GetCurrentProgram() { return 0; }
	virtual void SetCurrentProgram(int32) { }

	virtual PlugParamIndex GetNumParameters() const { return kI3DL2ReverbNumParameters; }
	virtual PlugParamValue GetParameter(PlugParamIndex index);
	virtual void SetParameter(PlugParamIndex index, PlugParamValue value);

	virtual void Resume();
	virtual void Suspend() { m_isResumed = false; }
	virtual void PositionChanged();
	virtual bool IsInstrument() const { return false; }
	virtual bool CanRecieveMidiEvents() { return false; }
	virtual bool ShouldProcessSilence() { return true; }

#ifdef MODPLUG_TRACKER
	virtual CString GetDefaultEffectName() { return _T("I3DL2Reverb"); }

	virtual CString GetParamName(PlugParamIndex param);
	virtual CString GetParamLabel(PlugParamIndex);
	virtual CString GetParamDisplay(PlugParamIndex param);

	virtual CString GetCurrentProgramName() { return CString(); }
	virtual void SetCurrentProgramName(const CString &) { }
	virtual CString GetProgramName(int32) { return CString(); }

	virtual bool HasEditor() const { return false; }
#endif

	virtual void BeginSetProgram(int32) { }
	virtual void EndSetProgram() { }

	virtual int GetNumInputChannels() const { return 2; }
	virtual int GetNumOutputChannels() const { return 2; }

protected:
	float Room() const { return -10000.0f + m_param[kI3DL2ReverbRoom] * 10000.0f; }
	float RoomHF() const { return -10000.0f + m_param[kI3DL2ReverbRoomHF] * 10000.0f; }
	float RoomRolloffFactor() const { return m_param[kI3DL2ReverbRoomRolloffFactor] * 10.0f; }
	float DecayTime() const { return 0.1f + m_param[kI3DL2ReverbDecayTime] * 19.9f; }
	float DecayHFRatio() const { return 0.1f + m_param[kI3DL2ReverbDecayHFRatio] * 1.9f; }
	float Reflections() const { return -10000.0f + m_param[kI3DL2ReverbReflections] * 11000.0f; };
	float ReflectionsDelay() const { return m_param[kI3DL2ReverbReflectionsDelay] * 0.3f; }
	float Reverb() const { return -10000.0f + m_param[kI3DL2ReverbReverb] * 12000.0f; };
	float ReverbDelay() const { return m_param[kI3DL2ReverbReverbDelay] * 0.1f; }
	float Diffusion() const { return m_param[kI3DL2ReverbDiffusion] * 100.0f; }
	float Density() const { return m_param[kI3DL2ReverbDensity] * 100.0f; }
	float HFReference() const { return 20.0f + m_param[kI3DL2ReverbHFReference] * 19980.0f; }
	uint32 Quality() const { return Util::Round<uint32>(m_param[kI3DL2ReverbQuality] * 3.0f); }

	void RecalculateI3DL2ReverbParams();

	void SetDelayTaps();
	void SetDecayCoeffs();
	float CalcDecayCoeffs(int32 index);
};

} // namespace DMO

OPENMPT_NAMESPACE_END

#endif // !NO_PLUGINS