#ifndef __YIELD_HPP__
#define __YIELD_HPP__

#ifndef __GNUC__
template<typename T> class Yield {
protected:
	Yield() { __line__ = 0; }
	void Init() { __line__ = 0; }
	unsigned short __line__; 
	T __ret__;

public:
	const T & Current() const { return __ret__; }
};
template<> class Yield<void> {
protected:
	Yield() { Init(); }
	void Init() { __line__ = 0; }
	unsigned short __line__; 
};
#define Y_BEGIN()         switch (this->__line__) { case 0:
#define Y_YIELD0()                               do { this->__line__ = __LINE__; return true; case __LINE__: ; } while (0)
#define Y_YIELD1(__r__)   do { this->__ret__ = __r__; this->__line__ = __LINE__; return true; case __LINE__: ; } while (0)
#define Y_BREAK()         do { this->__line__ = 0; return false; } while (0)
#define Y_END()           } this->__line__ = 0; return false
#else
template<typename T> class Yield {
protected:
	Yield() { Init(); }
	void Init() { __line__ = nullptr; }
	void *__line__; 
	T __ret__;

public:
	const T & Current() const { return __ret__; }
};
template<> class Yield<void> {
protected:
	Yield() { Init(); }
	void Init() { __line__ = nullptr; }
	void *__line__; 
};
#define Y_BEGIN()         do { if (this->__line__) goto *(this->__line__); } while (0)
#define Y_YIELD0()        do { __label__ lbl;                        this->__line__ = &&lbl; return true; lbl: ; } while (0)
#define Y_YIELD1(__r__)   do { __label__ lbl; this->__ret__ = __r__; this->__line__ = &&lbl; return true; lbl: ; } while (0)
#define Y_BREAK()         do { this->__line__ = nullptr; return false; } while (0)
#define Y_END()           do { this->__line__ = nullptr; return false; } while (0)
#endif

#endif
