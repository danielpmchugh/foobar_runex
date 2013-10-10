//Smartptr.h	include file for various kind of smart pointers
//
//		first version July 2003
//		1.1		- october 2003: added deletion functor
//

#pragma once
#include "stdafx.h"
namespace Safe{


	//***********************************************************
	//the exception thrown if trying to dereference a null smartptr
	struct XRefcnt_ptr_excp
	{
		static void Throw()  //just trow an XRefcnt_ptr_excp*
		{
			static XRefcnt_ptr_excp e;
			throw &e;
		};
	};

	//***********************************************************

	
	//anticipate the pointers declarations
	template <class T>	class _PtrSmart;
	template <class T, class C>	class PtrStrong;
	template <class T, class C>	class PtrWeak;


	// #### UPDATE 1.1
	////////////////////////
	// deletion functor: called to delete an object
	template<class T>
		struct FDeletor
		{
			// by default just calls delete. Specialize if you need different behaviour
			void operator()(T* pT) {delete pT;}
		};

	//common base (interface) for all referene counter

	namespace{ //unnamed namespace, to localize this internal data

		struct RefCount_base
		{
			virtual ~RefCount_base() {;}
			virtual void IncStrong()=0;
			virtual void IncWeak()=0;
			virtual bool ReleaseStrong()=0;
			virtual bool ReleaseWeak()=0;
			virtual bool IsNull()=0;
			virtual LONG GetRefCount()=0;
		};

		//the reference count structure
		template<class R>
		struct RefCount: public RefCount_base
		{
		private:
			LONG _cntStrong;
			LONG _cntWeak;
			R* _ptr;
		public:
			RefCount(R* pR) //creates reference counters
			{
				_cntStrong = _cntWeak = 0;
				_ptr = pR;
			}
			virtual void IncStrong()
			{	InterlockedIncrement(&_cntStrong);			}
			virtual void IncWeak()
			{	InterlockedIncrement(&_cntWeak);			}
			virtual bool ReleaseStrong()
			{
				InterlockedDecrement(&_cntStrong);
				if(! _cntStrong)
				{
					IncWeak(); //protecte this refcount from deleting
					if(_ptr)
					//	### Update 1.1
					//	delete _ptr; //NOTE: destructors may call ReleaseWeak;
						FDeletor<R>()(_ptr);
					// ### end
					_ptr = NULL;

					ReleaseWeak();  //end of protection: if weak goes to zero also delete this
					return true;
				}
				return false;
			}
			virtual bool ReleaseWeak()
			{
				InterlockedDecrement(&_cntWeak);
				if(! _cntWeak)
				{
					if(!_cntStrong) //nobody is looking at "this"
 						delete this;
				}
				return true;
			}
			
			virtual bool IsNull() {	return !_ptr;	};
			virtual LONG GetRefCount() { return _cntStrong;}

			R* GetPtr()	const {return _ptr;}
		};

		//the common base for all "samrt objects"
		//	NOTE:	a complicated object may have many bases and many smartpointer types to various subobjects
		//	each of the bases may be EObject derived, however, we must assure that only one refounter is present
		//	hence, ObjStrong_base is defined as "virtual"
		class ObjStrong_base
		{
		protected:
			RefCount<ObjStrong_base>* _pRfcnt;

			ObjStrong_base() 
			{
				_pRfcnt = new RefCount<ObjStrong_base>(this);
				_pRfcnt->IncWeak();
			}
		public:
			//### update 1.1
			virtual void Destroy() { if(!_pRfcnt->GetRefCount()) delete this; };
			//### end 

			virtual ~ObjStrong_base()
			{
				if(_pRfcnt)
					_pRfcnt->ReleaseWeak();
			}
		};

	}

	// ### UPDATE 1.1
	////////////////////////
	// deletion functor: specilization for ObjStrong_base
	//
	template<>
		struct FDeletor<ObjStrong_base>
		{
			// by default just calls delete. Specialize if you need different behaviour
			void operator()(ObjStrong_base* pT) {pT->Destroy();}
		};
	/// END
	
	
	//*******************************************************************************
	//use this base for objects you want to be convertible from "dumb" to "smart" or "weak" pointers
	//	derive every object you wanto to be saf in conversion between "dumb" and "smart" poiters 

	class ObjStrong: public virtual ObjStrong_base
	{
	private:
		LPARAM _signature;
		static LPARAM GetSignature() {static int foo; return (LPARAM) &foo;};
	public:
		bool IsStrong() const {return _signature == GetSignature();}
		ObjStrong() {_signature = GetSignature(); }
		RefCount_base* GetRefCounters() const {return _pRfcnt;}
		LONG GetRefCount() const {return _pRfcnt->GetRefCount();}
	};

	namespace {
		
		//global helper

		template<class T>
		RefCount_base* GetRefCountBase(T* pT)
		{
			ObjStrong* pStrong = (ObjStrong*)pT; //unsafe cast
			if(!pStrong->IsStrong()) //unmatched signature
				return NULL;
			return pStrong->GetRefCounters();
			return NULL;
		}

	}

	//*******************************************************************************
	//the reference couter smart pointers bases
	
	//for convenience a same base is defined for all smart pointers
	class _PtrSmart_base{};

	//a typed common base for all sart pointers
	template<class T>
	class _PtrSmart: public _PtrSmart_base
	{
		friend class PtrStrong;
		friend class PtrWeak;
		friend class XAggregator;
	protected:
		T* _pT; //the referenced object; NOTE: it may be different that the referred by RefCount, because it may be a sub or super object
		RefCount_base* _pRefCnt;

		//initialization
		void _Init()
		{
			_pT = NULL;
			_pRefCnt = NULL;
		}
		_PtrSmart() { _Init();	}
		virtual ~_PtrSmart() {;}
	public:
		bool IsNull() const { return !_pRefCnt || _pRefCnt->IsNull(); }
		//dereferencing
		operator T*() const {return _pT;}
		T* operator->() const { if(IsNull()) XRefcnt_ptr_excp::Throw(); return _pT; }
		T& operator*() const { if(IsNull()) XRefcnt_ptr_excp::Throw(); return *_pT; }
		bool operator!() const { return IsNull(); }
		LONG GetRefCount() { return _pRefCnt->GetRefCount(); }
		virtual Set(T* pT)=0;

		//the behaviour changes from strong to weak
	};


	//here is a default for PtrConvert, doing nothing
	// you can customize this if you have lots of convertion fron a signle type
	//specilizing the calls for the "From" class, and the operaqtor for the "To" classes
	template<class U> //from type
	struct PtrConvert
	{
		template<class T> //to type
		void operator() (T*& pT, U*pU) const
		{	pT = NULL;	} //default does not convert
	};

	//and here is a template function, doing nothing
	// you can specialize for "To - From" types couple
	template<class T, class U>
	T* FPtrConvertFn(U* pU)
	{
		return NULL;
	};

	//here are the type casting functors
	template<class T>
	struct FDynamicCast
	{
		template<class U>
		T operator() (U u) {return dynamic_cast<T>(u);}
	};

	template<class T>
	struct FStaticCast
	{
		template<class U>
		T operator() (U u) {return static_cast<T>(u);}
	};


	template<
		class T,   //the "to" class
		class U,   //the "from" class
		class C  //the default "caster"
	>	
	T* smart_cast(U* pU)
	{
		T* pT = NULL;
		
		//try using the function
		pT = FPtrConvertFn<T,U>(pU);
		if(pT)
			return pT;	//successful conversion
		
		//try using functional
		PtrConvert<U>()(pT, pU);
		if(pT) 
			return pT; //successfull conversion

		return C()(pU);
	};


	//####################################
	//	PtrStrong
	//	it can refer "smart"(ObjStrong derived) or "weak" objects.
	//	It can also do comparison between poiters and objects

	template
	<
		class T,		//the class this pointer is for
#ifdef _CPPRTTI
		class C = FDynamicCast<T*>  //the type caster towards T*
#else
		class C = FStaticCast<T*>
#endif
	>		
	class PtrStrong: public _PtrSmart<T>
	{
		friend class _PtrSmart;

	private:

		void _Clear()
		{
			if(_pRefCnt)
				_pRefCnt->ReleaseStrong();
			_pRefCnt = NULL;
			_pT = NULL;
		}

		void _Assign(T* pT)
		{
			if(_pT == pT) return;

			_Clear();

			if(pT)
			{
				_pRefCnt = GetRefCountBase<T>(pT);
				if(!_pRefCnt) 
				{
					//may be the object is not "smart", 
					//or no provision from aggregation is made.
					//	do a new refcounter (## less safe! ##)
					_pRefCnt = new RefCount<T>(pT);
				}
				_pRefCnt->IncStrong();
				_pT = pT;
			}
		}

		template<class U>
		void _Assign(U* pU)
		{
			T* pT = smart_cast<T,U,C>(pU);
			if(_pT == pT) return;

			_Clear();

			if(pT)
			{
				_pRefCnt = GetRefCountBase<U>(pU);
				if(!_pRefCnt) 
				{
					//may be the object is not "smart", 
					//or no provision from aggregation is made.
					//	do a new refcounter (## less safe! ##)
					_pRefCnt = new RefCount<U>(pU);
				}
				_pRefCnt->IncStrong();
				_pT = pT;
			}
		}

		void _Assign(const _PtrSmart<T>& cpsT)
		{
			bool bToNull = cpsT.IsNull();
			if(!bToNull && _pT == cpsT._pT) return;

			_Clear();
			if(!bToNull)
			{
				_pT = cpsT._pT;
				_pRefCnt = cpsT._pRefCnt;
				_pRefCnt->IncStrong();
			}
		}

		template<class U>
			void _Assign(const _PtrSmart<U>& cpsU)
		{
			bool bToNull = cpsU.IsNull();
			T* pT = NULL; 
			if(!bToNull)
				pT = smart_cast<T,U,C>(cpsU._pT);
			if(pT && _pT == pT) return;

			_Clear();

			if(pT)
			{
				//use the same counter, but refer _pT
				_pRefCnt = cpsU._pRefCnt;
				_pT = pT;
				_pRefCnt->IncStrong();
			}
		}
		
	public:
		//public "user" functions
		
		//cunstructor, assignment and conversions
		PtrStrong() {;}
		~PtrStrong() {_Clear();};
		PtrStrong(const PtrStrong& cpsT) {_Assign(cpsT);}
		PtrStrong(const _PtrSmart<T>& cpsT) {_Assign(cpsT);}
		PtrStrong(const T* pT) {_Assign((T*)pT);}
		template<class U> PtrStrong(const U* pU) {_Assign<U>((U*)pU);}
		template<class U> PtrStrong(const _PtrSmart<U>& cpsU) {_Assign<U>(cpsU);}
		PtrStrong<T>& operator=(const PtrStrong& cpsT) {_Assign(cpsT); return *this;}
		PtrStrong<T>& operator=(const _PtrSmart<T>& cpsT) {_Assign(cpsT); return *this;}
		PtrStrong<T>& operator=(const T* pT) {_Assign((T*)pT); return *this;};
		template<class U> PtrStrong<T>& operator=(const U* pU) {_Assign<U>((U*)pU); return *this;}
		template<class U> PtrStrong<T>& operator=(const _PtrSmart<U>& cpsU) {_Assign<U>(cpsU); return *this;}

		T* New() {T* pT = new T; _Assign(pT); return pT;}
		template<class U> T* New(const U& u) {T* pT = new T(u); _Assign(pT); return pT;}
		template<class U, class V> T* New(const U& u, const V& v) {T* pT = new T(u,v); _Assign(pT); return pT;}
		template<class U, class V, class W> T* New(const U& u, const V& v, const W& w) {T* pT = new T(u,v,w); _Assign(pT); return pT;}
		void Null() {_Clear();};
		virtual Set(T* pT) { _Assign(pT); }
	};


	//####################################
	//	PtrWeak
	//	it can refer "smart"(ObjStrong derived) or "weak" objects.
	//	It can also do cpomparison between poiters and objects

	template
		<
			class T,				//the class this pointer is for
#ifdef _CPPRTTI
		class C = FDynamicCast<T*>  //the type caster towards T*
#else
		class C = FStaticCast<T*>
#endif
		>
	class PtrWeak: public _PtrSmart<T>
	{
		void _Clear()
		{
			if(_pRefCnt)
				_pRefCnt->ReleaseWeak();
			_pRefCnt = NULL;
			_pT = NULL;
		}

		void _Assign(T* pT)
		{
			if(_pT == pT) return;

			_Clear();

			if(pT)
			{
				_pRefCnt = GetRefCountBase<T>(pT);
				if(!_pRefCnt) 
				{
					//may be the object is not "smart", 
					//or no provision from aggregation is made.
					//	do a new refcounter (## less safe! ##)
					_pRefCnt = new RefCount<T>(pT);
				}
				_pRefCnt->IncWeak();
				_pT = pT;
			}
		}

		template<class U>
		void _Assign(U* pU)
		{
			T* pT = smart_cast<T,U>(pT, pU);
			if(_pT == pT) return;

			_Clear();

			if(pT)
			{
				_pRefCnt = GetRefCountBase<U>(pU);
				if(!_pRefCnt) 
				{
					//may be the object is not "smart", 
					//or no provision from aggregation is made.
					//	do a new refcounter (## less safe! ##)
					_pRefCnt = new RefCount<U>(pU);
				}
				_pRefCnt->IncWeak();
				_pT = pT;
			}
		}

		void _Assign(const _PtrSmart<T>& cpsT)
		{
			bool bToNull = cpsT.IsNull();
			if(!bToNull && _pT == cpsT._pT) return;

			_Clear();
			if(!bToNull)
			{
				_pT = cpsT._pT;
				_pRefCnt = cpsT._pRefCnt;
				_pRefCnt->IncWeak();
			}
		}

		template<class U>
			void _Assign(const _PtrSmart<U>& cpsU)
		{
			bool bToNull = cpsU.IsNull();
			T* pT = NULL; 
			if(!bToNull)
				pT = smart_cast<T,U>(cpsU._pT);
			if(pT && _pT == pT) return;

			_Clear();

			if(pT)
			{
				//use the same counter, but refer _pT
				_pRefCnt = cpsU._pRefCnt;
				_pT = pT;
				_pRefCnt->IncWeak();
			}
		}
		
	public:

		//public "user" functions
		
		PtrWeak() {;}
		~PtrWeak() {_Clear();};
		PtrWeak(const PtrWeak& cpsT) {_Assign(cpsT);}
		PtrWeak(const _PtrSmart<T>& cpsT) {_Assign(cpsT);}
		PtrWeak(const T* pT) {_Assign((T*)pT);}
		template<class U> PtrWeak(const U* pU) {_Assign<U>((U*)pU);}
		template<class U> PtrWeak(const _PtrSmart<U>& cpsU) {_Assign<U>(cpsU);}
		PtrWeak<T>& operator=(const PtrWeak& cpsT) {_Assign(cpsT); return *this;}
		PtrWeak<T>& operator=(const _PtrSmart<T>& cpsT) {_Assign(cpsT); return *this;}
		PtrWeak<T>& operator=(const T* pT) {_Assign((T*)pT); return *this;};
		template<class U> PtrWeak<T>& operator=(const U* pU) {_Assign<U>((U*)pU); return *this;}
		template<class U> PtrWeak<T>& operator=(const _PtrSmart<U>& cpsU) {_Assign<U>(cpsU); return *this;}
		virtual Set(T* pT) { _Assign(pT); }
	};


	//comparison

	//****** dafault comparison implementation ******
	
	//equal is by pointers. specialize differently if it is the case
	template<class T>
		bool SmartPtrEqual(T* p1, T* p2)
	{	return std::equal_to<T*>()(p1,p2);	}

	//less is by referred value. Specialize it differently if it is the case
	template<class T>
		bool SmartPtrLess(T* p1, T* p2)
	{	try {return std::less<T>()(*p1,*p2);} catch(...){;} return false;	}

    template<class T>
		bool operator == (const _PtrSmart<T>& p1, const _PtrSmart<T>& p2)
	{	return SmartPtrEqual<T>(p1, p2);	};

    template<class T>
		bool operator == (const _PtrSmart<T>& p1, const T* p2)
	{	return SmartPtrEqual<T>(p1, p2);	};

    template<class T>
		bool operator == (const T* p1, const _PtrSmart<T>& p2)
	{	return SmartPtrEqual<T>(p1, p2);	};

    template<class T>
		bool operator < (const _PtrSmart<T>& p1, const _PtrSmart<T>& p2)
	{	return SmartPtrLess<T>(p1, p2);	};

    template<class T>
		bool operator < (const _PtrSmart<T>& p1, const T* p2)
	{	return SmartPtrLess<T>(p1, p2);	};

    template<class T>
		bool operator < (const T* p1, const _PtrSmart<T>& p2)
	{	return SmartPtrLess<T>(p1, p2);	};

}






