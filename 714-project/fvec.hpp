#pragma once
#include <immintrin.h>

template<typename vec_ty,int v_size>
class fvec{
public:
	// static constexpr int a_vsize = v_size;
	using num_ty = typename vec_ty::num_ty;
	vec_ty vec[v_size];
	explicit fvec(num_ty num){
		vec_ty cons(num);
		for (int g = 0;g < v_size;g++)
			vec[g] = cons;
	}
	explicit fvec(num_ty * src){
		int vec_size = vec_ty().size();
		for (int g = 0;g < v_size;g++)
			vec[g] = vec_ty(src + g * vec_size);
	}
    fvec() = default;

	static constexpr size_t size(){
		return v_size * vec_ty::size();
	}
	num_ty sum(){
		vec_ty sum = vec[0];
		for (int g = 1;g < v_size;g++)
			sum += vec[g];

		return sum.sum();
	}
	vec_ty accm(){
		vec_ty sum = vec[0];
		for (int g = 1;g < v_size;g++)
			sum += vec[g];
		return sum;
	}
	fvec aprox_recip(){
		for (int g = 0;g < v_size;g++)
			vec[g].aprox_recip();
	}
	void store(num_ty * dest){
		int vec_size = vec_ty().size();
		for (int g = 0;g < v_size;g++)
			vec[g].store(dest + g * vec_size);
	}
	num_ty * begin(){
		return reinterpret_cast<num_ty *>(this);
	}
	num_ty * end(){
		return reinterpret_cast<num_ty *>(this) + size();
	}
	template<typename fn_ty>
	fvec call_comnts(fn_ty fn){
		fvec ne;
		for (int g = 0;g < v_size;g++)
			ne.vec[g] = fn(vec[g]);
		return ne;
	}
	template<typename fn_ty>
	fvec call_elements(fn_ty fn){
		fvec ne = *this;
		for (num_ty & n : ne)
			n = fn(n);
		return ne;
	}

#define op_fns(op) \
	fvec & operator op##= (const fvec & other){\
		for (int g = 0;g < v_size;g++)\
			vec[g] op##= other.vec[g];\
        return *this;\
    }\
	fvec operator op (const fvec & other){\
		fvec res;\
		for (int g = 0;g < v_size;g++)\
			res.vec[g] = vec[g] op other.vec[g];\
		return res;\
    }\
	fvec & operator op##= (const vec_ty & other_vec){\
	for (int g = 0;g < v_size;g++)\
		vec[g] op##= other_vec;\
        return *this;\
    }\
	fvec operator op (const vec_ty & other_vec){\
		fvec res;\
		for (int g = 0;g < v_size;g++)\
			res.vec[g] = vec[g] op other_vec;\
		return res;\
    }
    op_fns(+)
    op_fns(-)
    op_fns(*)
    op_fns(/)
};
template<typename vec_ty,int v_size>
fvec<vec_ty,v_size> fma (const fvec<vec_ty,v_size> & x,const fvec<vec_ty,v_size> & y,const fvec<vec_ty,v_size> & z){
	fvec<vec_ty,v_size> res;
	for (int g = 0;g < v_size;g++){
		res.vec[g] = fma(x.vec[g],y.vec[g],z.vec[g]);
	}
	return res;
}
template<typename vec_ty,int v_size>
fvec<vec_ty,v_size> fma (const fvec<vec_ty,v_size> & x,const fvec<vec_ty,v_size> & y,const vec_ty & z){
	fvec<vec_ty,v_size> res;
	for (int g = 0;g < v_size;g++){
		res.vec[g] = fma(x.vec[g],y.vec[g],z);
	}
	return res;
}
template<typename vec_ty,int v_size>
fvec<vec_ty,v_size> fma (const fvec<vec_ty,v_size> & x,const vec_ty& y,const fvec<vec_ty,v_size> & z){
	fvec<vec_ty,v_size> res;
	for (int g = 0;g < v_size;g++){
		res.vec[g] = fma(x.vec[g],y,z.vec[g]);
	}
	return res;
}
template<typename vec_ty,int v_size>
fvec<vec_ty,v_size> fma (const vec_ty & x,const fvec<vec_ty,v_size> & y,const fvec<vec_ty,v_size> & z){
	fvec<vec_ty,v_size> res;
	for (int g = 0;g < x.v_size;g++){
		res.vec[g] = fma(x,y.vec[g],z.vec[g]);
	}
	return res;
}


class fvec8
{
public:
	typedef float num_ty;

    explicit fvec8(float A,float B,float C,float D,float E,float F,float G,float H){
		d = _mm256_set_ps(A,B,C,D,E,F,G,H);
    }
    explicit fvec8(float num){
        d = _mm256_set1_ps(num);
    }
    explicit fvec8(float * src){
        d = _mm256_loadu_ps(src);
    }
    fvec8(__m256 d_in){
        d = d_in;
    }
    fvec8(){
        d = _mm256_setzero_ps();
    }

    __m256 d;
    fvec8 & operator += (const fvec8 & other){
        d = _mm256_add_ps(d,other.d);
        return *this;
    }
    fvec8 & operator *= (const fvec8 & other){
        d = _mm256_mul_ps(d,other.d);
        return *this;
    }
    fvec8 & operator -= (const fvec8 & other){
        d = _mm256_sub_ps(d,other.d);
        return *this;
    }
    fvec8 & operator /= (const fvec8 & other){
        d = _mm256_div_ps(d,other.d);
        return *this;
    }
    fvec8 operator + (const fvec8 & other){
        return _mm256_add_ps(d,other.d);
    }
    fvec8 operator - (const fvec8 & other){
        return _mm256_sub_ps(d,other.d);
    }
    fvec8 operator * (const fvec8 & other){
        return _mm256_mul_ps(d,other.d);
    }
	fvec8 operator / (const fvec8 & other){
		return _mm256_div_ps(d,other.d);
	}
    fvec8 aprox_recip(){
        return _mm256_rcp_ps(d);
	}
    void store(float * dest){
        _mm256_storeu_ps(dest,d);
    }
    float sum(){
        __m128 top = _mm256_extractf128_ps(d,1);
        __m128 bottom = _mm256_castps256_ps128(d);
        top = _mm_add_ps(top,bottom);
        float ds[4];
        _mm_storeu_ps(ds,top);
        ds[0] += ds[2];
        ds[1] += ds[3];
        ds[0] += ds[1];
        return ds[0];
    }
    static constexpr size_t size(){
		return 8;
	}
	float * begin(){
		return reinterpret_cast<float *>(this);
	}
	float * end(){
		return reinterpret_cast<float *>(this) + size();
	}
};
fvec8 fma (const fvec8 & x,const fvec8 & y,const fvec8 & z){
#ifdef __FMA__
	return _mm256_fmadd_ps(x.d,y.d,z.d);
#else
	return _mm256_add_ps(_mm256_mul_ps(x.d,y.d),z.d);
#endif
}
inline fvec8 max(const fvec8 & one,const fvec8 & two){
    return _mm256_max_ps(one.d,two.d);
}
inline fvec8 sqrt(const fvec8 & x){
    return _mm256_sqrt_ps(x.d);
}

class fvec4
{
public:
	typedef float num_ty;

    explicit fvec4(float a,float b,float c,float d){
        this->d = _mm_set_ps(a,b,c,d);
    }
    explicit fvec4(float num){
        d = _mm_set1_ps(num);
    }
    explicit fvec4(float * src){
        d = _mm_loadu_ps(src);
    }
    fvec4(__m128 d_in){
        d = d_in;
    }
    fvec4(){
        d = _mm_setzero_ps();
    }

    __m128 d;
    fvec4 & operator += (const fvec4 & other){
        d = _mm_add_ps(d,other.d);
        return *this;
    }
    fvec4 & operator *= (const fvec4 & other){
        d = _mm_mul_ps(d,other.d);
        return *this;
    }
    fvec4 & operator -= (const fvec4 & other){
        d = _mm_sub_ps(d,other.d);
        return *this;
    }
    fvec4 & operator /= (const fvec4 & other){
        d = _mm_div_ps(d,other.d);
        return *this;
    }
    fvec4 operator + (const fvec4 & other){
        return _mm_add_ps(d,other.d);
    }
    fvec4 operator - (const fvec4 & other){
        return _mm_sub_ps(d,other.d);
    }
    fvec4 operator * (const fvec4 & other){
        return _mm_mul_ps(d,other.d);
    }
    fvec4 operator / (const fvec4 & other){
        return _mm_div_ps(d,other.d);
    }
    fvec4 aprox_recip(){
        return _mm_rcp_ps(d);
	}
    void store(float * dest){
        _mm_storeu_ps(dest,d);
    }
	static constexpr size_t size(){
		return 4;
	}
    float sum(){
        float ds[4];
        _mm_storeu_ps(ds,d);
        ds[0] += ds[2];
        ds[1] += ds[3];
        ds[0] += ds[1];
        return ds[0];
    }
	float * begin(){
		return reinterpret_cast<float *>(this);
	}
	float * end(){
		return reinterpret_cast<float *>(this) + size();
	}
};
inline fvec4 max(const fvec4 & one,const fvec4 & two){
    return _mm_max_ps(one.d,two.d);
}
inline fvec4 sqrt(const fvec4 & x){
    return _mm_sqrt_ps(x.d);
}
fvec4 fma (const fvec4 & x,const fvec4 & y,const fvec4 & z){
#ifdef __FMA__
	return _mm_fmadd_ps(x.d,y.d,z.d);
#else
	return _mm_add_ps(_mm_mul_ps(x.d,y.d),z.d);
#endif
}


class fvec1
{
public:
	typedef float num_ty;

    explicit fvec1(float * src){
        d = *(src);
    }
    fvec1(float d_in){
        d = d_in;
    }
    fvec1(){
        d = 0;
    }

    float d;
    fvec1 & operator += (const fvec1 & other){
        d = (d+other.d);
        return *this;
    }
    fvec1 & operator *= (const fvec1 & other){
        d = (d*other.d);
        return *this;
    }
    fvec1 & operator -= (const fvec1 & other){
        d = (d-other.d);
        return *this;
    }
    fvec1 & operator /= (const fvec1 & other){
        d = (d/other.d);
        return *this;
    }
    fvec1 operator + (const fvec1 & other){
        return (d+other.d);
    }
    fvec1 operator - (const fvec1 & other){
        return (d-other.d);
    }
    fvec1 operator * (const fvec1 & other){
        return (d*other.d);
    }
    fvec1 operator / (const fvec1 & other){
        return (d/other.d);
    }
    fvec1 aprox_recip(){
        return 1/(d);
	}
    void store(float * dest){
        *dest = d;
    }
	static constexpr size_t size(){
		return 1;
	}
    float sum(){
        return d;
    }
	float * begin(){
		return reinterpret_cast<float *>(this);
	}
	float * end(){
		return reinterpret_cast<float *>(this) + size();
	}
};
inline fvec1 max(const fvec1 & one,const fvec1 & two){
    return std::max(one.d,two.d);
}
inline fvec1 sqrt(const fvec1 & x){
    return sqrt(x.d);
}
fvec1 fma (const fvec1 & x,const fvec1 & y,const fvec1 & z){
    return x.d * y.d + z.d;
}
