#pragma once

#include "hrs_reset_buffers.h"
#include "RadeonProRender_V2.h"


class HorusPostEffect
{
public:

	static HorusPostEffect& GetInstance()
	{
		static HorusPostEffect Instance;
		return Instance;
	}

	HorusPostEffect(const HorusPostEffect&) = delete;
	void operator=(const HorusPostEffect&) = delete;

	void CreatePostEffect(rpr_context context, rpr_post_effect_type type, rpr_post_effect* out_effect);

	void AttachPostEffect(rpr_context context, rpr_post_effect effect);
	void DetachPostEffect(rpr_context context, rpr_post_effect effect);

	void SetPostEffectParameter1u(rpr_context context, rpr_post_effect effect, rpr_char const * name, rpr_uint x);
	void SetPostEffectParameter1f(rpr_context context, rpr_post_effect effect, rpr_char const * name, rpr_float x);
	void SetPostEffectParameter3f(rpr_context context, rpr_post_effect effect, rpr_char const * name, rpr_float x, rpr_float y, rpr_float z);
	void SetPostEffectParameter4f(rpr_context context, rpr_post_effect effect, rpr_char const * name, rpr_float x, rpr_float y, rpr_float z, rpr_float w);

	void GetAttachedPostEffectCount(rpr_context context,rpr_uint out_nb);
	void GetAttachedPostEffect(rpr_context context, rpr_uint i, rpr_post_effect* out_effect);

	void GetPostEffectInfo(rpr_post_effect effect, rpr_post_effect_info info, size_t size, void * data, size_t * size_ret);




private:

	HorusPostEffect() {}



};

