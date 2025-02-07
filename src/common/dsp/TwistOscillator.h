/*
** Surge Synthesizer is Free and Open Source Software
**
** Surge is made available under the Gnu General Public License, v3.0
** https://www.gnu.org/licenses/gpl-3.0.en.html
**
** Copyright 2004-2021 by various individuals as described by the Git transaction log
**
** All source at: https://github.com/surge-synthesizer/surge.git
**
** Surge was a commercial product from 2004-2018, with Copyright and ownership
** in that period held by Claes Johanson at Vember Audio. Claes made Surge
** open source in September 2018.
*/

#include "OscillatorBase.h"
#include <memory>
#include "basic_dsp.h"
#include "DspUtilities.h"
#include "OscillatorCommonFunctions.h"

namespace plaits
{
class Voice;
class Patch;
} // namespace plaits

namespace stmlib
{
class BufferAllocator;
}

struct SRC_STATE_tag;

class TwistOscillator : public Oscillator
{
  public:
    enum twist_params
    {
        twist_engine,
        twist_harmonics,
        twist_timbre,
        twist_morph,
        twist_aux_mix,
        twist_lpg_response,
        twist_lpg_decay
    };

    TwistOscillator(SurgeStorage *storage, OscillatorStorage *oscdata, pdata *localcopy);
    ~TwistOscillator();

    void process_block(float pitch, float drift, bool stereo, bool FM, float FMdepth) override;

    template <bool FM, bool throwaway = false>
    void process_block_internal(float pitch, float drift, bool stereo, float FMdepth,
                                int throwawayBlocks = -1);

    virtual void init(float pitch, bool is_display = false, bool nonzero_drift = true) override;
    virtual void init_ctrltypes(int scene, int oscnum) override { init_ctrltypes(); };
    virtual void init_ctrltypes() override;
    virtual void init_default_values() override;
    float tuningAwarePitch(float pitch);

    // The value of the localcopy
    inline float fv(twist_params ps) { return localcopy[oscdata->p[ps].param_id_in_scene].f; }
    inline float fvbp(twist_params ps)
    {
        return limit_range((localcopy[oscdata->p[ps].param_id_in_scene].f + 1) * 0.5f, 0.f, 1.f);
    }

    std::unique_ptr<plaits::Voice> voice;
    std::unique_ptr<plaits::Patch> patch;
    std::unique_ptr<stmlib::BufferAllocator> alloc;
    char shared_buffer[16834];
    SRC_STATE_tag *srcstate, *fmdownsamplestate;
    float fmlagbuffer[BLOCK_SIZE_OS << 1];
    int fmwp, fmrp;

    float carryover[BLOCK_SIZE_OS][2];
    int carrover_size = 0;

    lag<float, true> harm, timb, morph, lpgcol, lpgdec, auxmix;

    Surge::Oscillator::DriftLFO driftLFO;
    Surge::Oscillator::CharacterFilter<float> charFilt;
};
