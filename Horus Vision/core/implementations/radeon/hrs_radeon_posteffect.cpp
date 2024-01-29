#include "hrs_radeon_posteffect.h"
#include "common.h"

#include "spdlog/spdlog.h"

void HorusPostEffect::CreatePostEffect(rpr_context context, rpr_post_effect_type type, rpr_post_effect* out_effect)
{
	CHECK(rprContextCreatePostEffect(context, type, out_effect));
	spdlog::info("Post effect created {}", type);
}
void HorusPostEffect::AttachPostEffect(rpr_context context, rpr_post_effect effect)
{
	CHECK(rprContextAttachPostEffect(context, effect));
	spdlog::info("Post effect attached");
}
void HorusPostEffect::DetachPostEffect(rpr_context context, rpr_post_effect effect)
{
	CHECK(rprContextDetachPostEffect(context, effect));
	spdlog::info("Post effect detached");
}

void HorusPostEffect::SetPostEffectParameter1u(rpr_context context, rpr_post_effect effect, rpr_char const* name, rpr_uint x)
{
	CHECK(rprPostEffectSetParameter1u(effect, name, x));
}
void HorusPostEffect::SetPostEffectParameter1f(rpr_context context, rpr_post_effect effect, rpr_char const* name, rpr_float x)
{
	CHECK(rprPostEffectSetParameter1f(effect, name, x));
}
void HorusPostEffect::SetPostEffectParameter3f(rpr_context context, rpr_post_effect effect, rpr_char const* name, rpr_float x, rpr_float y, rpr_float z)
{
	CHECK(rprPostEffectSetParameter3f(effect, name, x, y, z));
}
void HorusPostEffect::SetPostEffectParameter4f(rpr_context context, rpr_post_effect effect, rpr_char const* name, rpr_float x, rpr_float y, rpr_float z, rpr_float w)
{
	CHECK(rprPostEffectSetParameter4f(effect, name, x, y, z, w));
}

void HorusPostEffect::GetAttachedPostEffectCount(rpr_context context, rpr_uint out_nb)
{
	CHECK(rprContextGetAttachedPostEffectCount(context, &out_nb));
}
void HorusPostEffect::GetAttachedPostEffect(rpr_context context, rpr_uint i, rpr_post_effect* out_effect)
{
	CHECK(rprContextGetAttachedPostEffect(context, i, out_effect));
}
void HorusPostEffect::GetPostEffectInfo(rpr_post_effect effect, rpr_post_effect_info info, size_t size, void* data, size_t* size_ret)
{
	CHECK(rprPostEffectGetInfo(effect, info, size, data, size_ret));
}
