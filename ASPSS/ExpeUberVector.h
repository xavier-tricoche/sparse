/*
----------------------------------------------------------------------

This source file is part of Expé
(EXperimental Point Engine)

Copyright (c) 2004-2007 by
 - Computer Graphics Laboratory, ETH Zurich
 - IRIT, University of Toulouse
 - Gael Guennebaud.

----------------------------------------------------------------------

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.

http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
----------------------------------------------------------------------
*/



#ifndef _ExpeUberVector_h_
#define _ExpeUberVector_h_

#include "ExpeCore.h"
//#include <ext/algorithm>
#include <map>
#include <vector>
#include <algorithm>
#include <QString>
#include "ExpeMetaProgramming.h"

// #define UBERVECTOR_USE_MALLOC
#define UBERVECTOR_MEMMOVE memmove

#define ATTRIBUTE_custom (128)

namespace Expe
{

#define EXPE_UBERVECTOR_BEGIN_BUILTIN_ACCESSORS(NB) typedef FormatT<16> Format; \
    template <bool isConst> class BuiltinAccessors : public BaseAccessorT<isConst,Format> { \
        typedef BaseAccessorT<isConst,Format> Super; \
    public: \
        BuiltinAccessors(void) {} \
        BuiltinAccessors(Pointer ptr, const Format& fmt) : Super(ptr,fmt) {} \
        BuiltinAccessors(Pointer ptr, const Format* pFmt) : Super(ptr,pFmt) {} \

#define EXPE_UBERVECTOR_END_BUILTIN_ACCESSORS  }; \
    typedef AccessorT<BuiltinAccessors> ElementHandle; \
    typedef ConstAccessorT<BuiltinAccessors> ConstElementHandle;

/// \internal
#define EXPE_UBERVECTOR_GETTYPEREF_WITH_CONSTNESS(NAME) typename MTP::IF<isConst, \
    const _MetaAttribute_##NAME##_::Type&, _MetaAttribute_##NAME##_::Type&>::RET

/// \internal
#define EXPE_UBERVECTOR_GETTYPEPTR_WITH_CONSTNESS(NAME) typename MTP::IF<isConst, \
    const _MetaAttribute_##NAME##_::Type*, _MetaAttribute_##NAME##_::Type*>::RET 


/// Add the members related to the attribute NAME
#define EXPE_UBERVECTOR_BUILTIN_ACCESSOR(NAME)  \
    inline EXPE_UBERVECTOR_GETTYPEREF_WITH_CONSTNESS(NAME) NAME (void) const \
    { \
        assert(this->mpFormat->offsets[_MetaAttribute_##NAME##_::Id]!=0xffffffff); \
        return * ( EXPE_UBERVECTOR_GETTYPEPTR_WITH_CONSTNESS(NAME) )(this->mAddr + this->mpFormat->offsets[_MetaAttribute_##NAME##_::Id]); \
    } \
    inline EXPE_UBERVECTOR_GETTYPEREF_WITH_CONSTNESS(NAME) NAME (uint i) const \
    { \
        assert( i < NAME##_count ()); \
        return * ( EXPE_UBERVECTOR_GETTYPEPTR_WITH_CONSTNESS(NAME) )(this->mAddr + this->mpFormat->offsets[_MetaAttribute_##NAME##_::Id] + i*sizeof(_MetaAttribute_##NAME##_::Type)); \
    } \
    inline uint NAME##_count (void) const \
    { \
        return this->mpFormat->multiplicity(_MetaAttribute_##NAME##_::Id); \
    }

/// Declare a builtin attribute
#define EXPE_UBERVECTOR_DECLARE_BUILTIN_ATTRIBUTE(TYPE,NAME,ID)  \
    typedef MetaAttributeT< TYPE, ID > _MetaAttribute_##NAME##_; \
    static _MetaAttribute_##NAME##_   Attribute_##NAME;


typedef uint Mask;

template <class CoreData> class UberVectorBaseT
{
public:

    class Attribute;
    typedef std::vector<Attribute> AttributeList;
    
    struct MetaAttribute
    {
        MetaAttribute(const QString& _name, uint _id, Enum _type, uint _nofComponents, uint _typeSize)
            : name(_name), id(_id), componentType(_type), nofComponents(_nofComponents), typeSize(_typeSize)
        {}
        
        operator Attribute ()
        {
            return Attribute(this);
        }
        
        /** Multiplicity operator
        */
        friend Attribute operator * (uint multi, const MetaAttribute& metaAttrib)
        {
            assert(multi>0);
            return Attribute(metaAttrib,multi);
        }
        
        /** Multiplicity operator
        */
        Attribute operator * (uint multi) const
        {
            assert(multi>0);
            return Attribute(this,multi);
        }
        
        /** Alignement operator.
            \param align Alignement in bytes
        */
        Attribute operator () (uint align) const
        {
            return Attribute(this,1,align);
        }
        
        /** Create a list of Attribute with a single element
        */
        inline operator AttributeList () const
        {
            AttributeList al;
            al.push_back(Attribute(this));
            return al;
        }

        /** Merge two Attribute to build a list of Attribute
        */
        AttributeList operator | (const Attribute& a) const
        {
            AttributeList al;
            al.push_back(Attribute(this));
            al.push_back(a);
            return al;
        }
        
        AttributeList operator | (const MetaAttribute& a) const
        {
            AttributeList al;
            al.push_back(Attribute(this));
            al.push_back(Attribute(&a));
            return al;
        }

        /** Append an Attribute into a list of Attribute
        */
        friend AttributeList operator | (AttributeList al, const MetaAttribute& a)
        {
            al.push_back(Attribute(&a));
            return al;
        }
        
        /// Name of the attribute
        QString name;
        
        /// Unique id.
        uint id;
        
        /// Type of the components.
        Enum componentType;
        
        /// Number of base element composing the attribute type.
        uint nofComponents;
        
        /// Size in bytes of the attribute type.
        uint typeSize;
    };

    class Attribute
    {
    public:
        Attribute(const MetaAttribute& metaAttrib, uint multi = 1, uint align = 0)
            : mpMetaData(&metaAttrib), mMultiplicity(multi), mAlignement(0)
        {}
        
        Attribute(const MetaAttribute* pMetaAttrib, uint multi = 1, uint align = 0)
            : mpMetaData(pMetaAttrib), mMultiplicity(multi), mAlignement(0)
        {}

        bool operator == (const Attribute& a) const
        {
            return id() == a.id();
        }
        
        inline bool operator< (const Attribute& a) const
        {
            return (id() < a.id());
        }

        /** Create a list of Attribute with a single element
        */
        inline operator AttributeList () const
        {
            AttributeList al;
            al.push_back(*this);
            return al;
        }

        /** Merge two Attribute to build a list of Attribute
        */
        AttributeList operator | (const Attribute& a) const
        {
            AttributeList al;
            al.push_back(*this);
            al.push_back(a);
            return al;
        }

        /** Append an Attribute into a list of Attribute
        */
        friend AttributeList operator | (AttributeList al, const Attribute& a)
        {
            al.push_back(a);
            return al;
        }

        /** Remove an Attribute from a list of Attribute
        */
//         friend AttributeList operator - (AttributeList al, Attribute& a) const
//         {
//             typename AttributeList::iterator it = std::find(al.begin(),al.end(),a);
//             if(it!=al.end())
//             {
//                 al.erase(it);
//             }
//             return al;
//         }
        
        /// multiplicity operator
        friend Attribute operator * (uint multi, const Attribute& attrib)
        {
            assert(attrib.mMultiplicity==1);
            assert(multi>0);
            return Attribute(attrib.mpMetaData,multi,attrib.mAlignement);
        }
        
        /// multiplicity operator
        Attribute operator * (uint multi) const
        {
            assert(mMultiplicity==1);
            assert(multi>0);
            return Attribute(mpMetaData,multi,mAlignement);
        }
        
        /** Alignement operator.
            \param align Alignement in bytes
        */
        Attribute operator () (uint align) const
        {
            return Attribute(mpMetaData,mMultiplicity,align);
        }
        
        inline uint multiplicity(void) const {return mMultiplicity;}
        
        inline uint alignement(void) const {return mAlignement;}
        
        inline Enum componentType(void) const {return mpMetaData->componentType;}
        
        inline const QString& name(void) const {return mpMetaData->name;}
        
        inline uint nofComponents(void) const {return mpMetaData->nofComponents;}
        
        inline uint id(void) const {return mpMetaData->id;}
        
        /** Total size of the attribute in bytes.
            FIXME include multiplicity and alignement ?
        */
        inline uint size(void) const
        {
            return mpMetaData->typeSize;
        }

    protected:
        
        const MetaAttribute* mpMetaData;
        
        /// Number of occurences (usually 1).
        uint mMultiplicity;
        
        uint mAlignement;
    };

    /** Base class to access to any attribute.
    */
    class AttributeHandle
    {
    public:
        AttributeHandle(uint offset, uint multiplicity)
            : mOffset(offset), mMultiplicity(multiplicity)
        {
        }
        AttributeHandle(void)
            : mOffset(0)
        {
        }
        
        inline uint offset(void) const
        {
            return mOffset;
        }
        
        inline uint multiplicity(void) const
        {
            return mMultiplicity;
        }
        
    protected:
        uint mOffset;
        uint mMultiplicity;
    };

    template<class T> class AttributeHandleT : public AttributeHandle
    {
    public:
        AttributeHandleT(uint offset)
            : AttributeHandle(offset)
        {
        }
        
        AttributeHandleT(void)
        {
        }
    };

    template<class T,int T_Id> class MetaAttributeT : public MetaAttribute
    {
    public:
    
        typedef T Type;
        typedef AttributeHandleT<T> Handle;
        enum {Id=T_Id};

        MetaAttributeT(const QString& _name)
            : MetaAttribute(_name, T_Id, TypeInfo<typename TypeInfo<T>::ComponentType>::Id, TypeInfo<T>::NofComponents, sizeof(T))
        {
        }
    };


    //--------------------------------------------------------------------------------


    template <uint NofBuiltinAttribsT> class FormatT
    {
    public:
        FormatT(const AttributeList& as)
        {
            mAttributes = as;
            std::sort(mAttributes.begin(), mAttributes.end());
            computeOffsets();
        }
        
        inline void clear(const AttributeList& as)
        {
            mAttributes = as;
            std::sort(mAttributes.begin(), mAttributes.end());
            computeOffsets();
        }

        inline bool operator == (const FormatT& rkFormat)
        {
            return mMask == rkFormat.mMask;
        }

        inline bool hasAttribute(const Attribute& a) const
        {
            return (1 << a.id()) & mMask;
        }
        
        inline bool hasAttribute(const MetaAttribute& a) const
        {
            return (1 << a.id) & mMask;
        }

        inline bool hasAttribute(const QString& name) const
        {
            iterator attrib_iter = mAttributes.begin();
            while( (attrib_iter != mAttributes.end()) && (attrib_iter->name() != name) )
            {
                ++attrib_iter;
            }
            return (attrib_iter != mAttributes.end());
        }
        
        inline uint multiplicity(const Attribute& a) const
        {
//             if (a.id() < ATTRIBUTE_custom) // FIXME ATTRIBUTE_custom
//             {
//                 TODO
//             }
//             else
//             {
//                 TODO
//             }
            return mAttributeHandles[a.id()].multiplicity();
        }
        
        inline uint multiplicity(const MetaAttribute& a) const
        {
//             if (a.id() < ATTRIBUTE_custom) // FIXME ATTRIBUTE_custom
//             {
//                 TODO
//             }
//             else
//             {
//                 TODO
//             }
            return mAttributeHandles[a.id]->multiplicity();
        }
        
        inline uint multiplicity(int aid) const
        {
//             if (a.id() < ATTRIBUTE_custom) // FIXME ATTRIBUTE_custom
//             {
//                 TODO
//             }
//             else
//             {
//                 TODO
//             }
            return mAttributeHandles[aid]->multiplicity();
        }

        inline uint size(void) const
        {
            return mSize;
        }

//         template<class T,int T_Id> void addAttribute(MetaAttributeT<T,T_Id>& a)
        //template< class _MetaAttributeT > void addAttribute(_MetaAttributeT& a)
        void addAttribute(const Attribute& a)
        {
            if (!hasAttribute(a))
            {
                mMask |= (1 << a.id());
    
                mAttributes.push_back(a);
    
                mAttributeHandles[a.id()] = new AttributeHandle(mSize,1);
                if (a.id() < ATTRIBUTE_custom) // FIXME ATTRIBUTE_custom
                {
                    offsets[a.id()] = mSize;
                }
                mSize += a.size();
            }
        }

//         template<class T,int T_Id> typename AttributeT<T,T_Id>::Handle& getAttributeHandle(AttributeT<T,T_Id>& a) const
//         {
//             return *(typename  AttributeT<T,T_Id>::Handle*)(mAttributeHandles[a.id()]);
//         }

        const AttributeHandle& _getAttributeHandle(const Attribute& a) const
        {
            return *mAttributeHandles[a.id()];
        }

        const AttributeHandle& _getAttributeHandle(const QString& name) const
        {
            iterator attrib_iter = mAttributes.begin();
            while( (attrib_iter != mAttributes.end()) && (attrib_iter->name() != name) )
            {
                ++attrib_iter;
            }
            if(attrib_iter == mAttributes.end())
            {
//                 LOG_ERROR("Attribute " + name + " not find (abort)");
                exit(1);
            }
            return _getAttributeHandle(*attrib_iter);
        }

//         void toVertexDeclaration(VertexDeclaration& vd, BufferObject* pBuffer) const
//         {
//             for(typename AttributeList::const_iterator attrib_iter = mAttributes.begin() ; attrib_iter!=mAttributes.end() ; ++attrib_iter)
//             {
//                 // name <=> {buffer, type, size, offset, stride}
//                 vd[(*attrib_iter)->name] = VertexElement(
//                     pBuffer,
//                     (*attrib_iter)->elementType,
//                     (*attrib_iter)->nofElements,
//                     mAttributeHandles[(*attrib_iter)->id]->offset(),
//                     size() );
//             }
//         }

        const AttributeList& getAttributes(void) const
        {
            return mAttributes;
        }

    protected:
    
        void reset(void)
        {
            mMask = 0;
            mSize = 0;
            mAttributeHandles.clear();
            for (uint i=0 ; i<NofBuiltinAttribsT ; ++i)
            {
                offsets[i] = 0xffffffff;
            }
        }

        void computeOffsets()
        {
            mMask = 0;
            mSize = 0;
            mAttributeHandles.clear();

            for (typename AttributeList::iterator attribute_iter = mAttributes.begin() ; attribute_iter != mAttributes.end() ; ++attribute_iter)
            {
                uint attributeId = attribute_iter->id();

                mMask |= (1 << attributeId);

                mAttributeHandles[attributeId] = new AttributeHandle(mSize,attribute_iter->multiplicity());

                if(attributeId < ATTRIBUTE_custom)
                {
                    offsets[attributeId] = mSize;
                }

                mSize += attribute_iter->size() * attribute_iter->multiplicity();
            }
        }

    public:

        // loop over the properties
        typedef typename AttributeList::const_iterator iterator;

        inline iterator begin(void) const
        {
            return mAttributes.begin();
        }
        inline iterator end(void) const
        {
            return mAttributes.end();
        }

        // FIXME : mutable is required by the const member toVertexDeclaration
        // which do a [] access to the map ...
        mutable std::map<uint,AttributeHandle*> mAttributeHandles;

        AttributeList mAttributes;
        uint offsets[NofBuiltinAttribsT];

        uint mSize;
        Mask mMask;
    };

    //--------------------------------------------------------------------------------


    template <bool isConstT,class _Format> class BaseAccessorT
    {
    //friend class ContainerT;
    public:
        enum {isConst = isConstT};
        typedef typename MTP::IF<isConst,ConstPointer,Pointer>::RET PointerT;
        
        BaseAccessorT(void)
            : mAddr(0), mpFormat(0)
        {}

        BaseAccessorT(PointerT ptr, const _Format& fmt)
            : mAddr(ptr), mpFormat(&fmt)
        {}

        BaseAccessorT(PointerT ptr, const _Format* pFmt)
            : mAddr(ptr), mpFormat(pFmt)
        {}
        
        inline bool operator == (const BaseAccessorT& el)
        {
            return this->mAddr == el.mAddr;
        }

        inline bool operator != (const BaseAccessorT& el)
        {
            return this->mAddr != el.mAddr;
        }

        inline BaseAccessorT& operator = (const BaseAccessorT& el)
        {
            this->mAddr = el.mAddr;
            this->mpFormat = el.mpFormat;
            return *this;
        }

//         // generic attribute accessors
//         template<class T>
//         inline T& attribute(const AttributeHandleT<T>& ph)
//         {
//             //return * (T*)(mAddr + mFormat.customAttributeOffset[ph.id()]);
//             return * (T*)(this->mAddr + ph.offset());
//         }

        template<class T>
        inline typename MTP::IF<isConst,const T&,T&>::RET attribute(const AttributeHandleT<T>& ph) const
        {
            //return * (T*)(mAddr + mFormat.customAttributeOffset[ph.id()]);
            return * (T*)(this->mAddr + ph.offset());
        }
        
//         template<class T>
//         inline T& attribute(const AttributeHandleT<T>& ph, uint i)
//         {
//             assert(i<ph.multiplicity());
//             //return * (T*)(mAddr + mFormat.customAttributeOffset[ph.id()]);
//             return * (T*)(this->mAddr + ph.offset() + i*sizeof(T));
//         }
        
        template<class T>
        inline typename MTP::IF<isConst,const T&,T&>::RET attribute(const AttributeHandleT<T>& ph, uint i) const
        {
            assert(i<ph.multiplicity());
            //return * (T*)(mAddr + mFormat.customAttributeOffset[ph.id()]);
            return * (T*)(this->mAddr + ph.offset() + i*sizeof(T));
        }

        inline PointerT attributePtr(const AttributeHandle& ph) const
        {
            return (this->mAddr + ph.offset());
        }

        /// \internal
        inline PointerT _getAddr(void) const
        {
            return this->mAddr;
        }

    protected:
        typedef _Format Format;
        typedef const Format* ConstFormatPtr;

        PointerT mAddr;
        ConstFormatPtr mpFormat;
    };
    
    // forward declaration
    template <template<bool> class BuiltInAccessorT> class AccessorT;
    
    /** Specific const accessors.
    */
    template <template<bool> class BuiltInAccessorT>
    class ConstAccessorT : public BuiltInAccessorT<true>
    {
        typedef BuiltInAccessorT<true> Super;
        typedef typename Super::Format Format;
    public:
        friend class AccessorT<BuiltInAccessorT>;
        ConstAccessorT(void) {}
        ConstAccessorT(ConstPointer ptr, const Format& fmt) : Super(ptr,fmt) {}
        ConstAccessorT(const ConstAccessorT& a) : Super(a) {}
    };
    
    /** Specific non-const accessors.
    */
    template <template<bool> class BuiltInAccessorT>
    class AccessorT : public BuiltInAccessorT<false>
    {
    
    typedef BuiltInAccessorT<false> Super;
    typedef typename Super::PointerT PointerT;
    typedef typename Super::Format Format;

    public:
    
#ifdef WIN32
		typedef ConstAccessorT<BuiltInAccessorT> ConstAccessorT;
#else
		typedef Expe::UberVectorBaseT<CoreData>::ConstAccessorT<BuiltInAccessorT> ConstAccessorT;
#endif

        AccessorT(void) {}
        AccessorT(PointerT ptr, const Format& fmt) : Super(ptr,fmt) {}
        AccessorT(const AccessorT& el) : Super(el.mAddr, el.mpFormat) {}
        
        inline operator ConstAccessorT& () { return *reinterpret_cast<ConstAccessorT*>(this); }

        /** Copy all properties of el into this
        */
        inline const AccessorT& copyFrom(const ConstAccessorT& el)
        {
            if(Super::mAddr != el.mAddr)
            {
                if(Super::mpFormat == el.mpFormat)
                {
                    // simply do a memcopy !
                    //std::cout << "copyFrom:: single memcopy\n";
                    UBERVECTOR_MEMMOVE(this->mAddr, el.mAddr, this->mpFormat->size());
                }
                else
                {
                    //std::cout << "copyFrom:: one by one\n";
                    // copy properties one by one
                    // loop on the element which have the minimum number of properties is more efficient
                    for(typename Format::iterator attrib_iter = this->mpFormat->begin() ;
                        attrib_iter!=this->mpFormat->end() ;
                        ++attrib_iter)
                    {
                        if(el.mpFormat->hasAttribute(*attrib_iter))
                        {
                            // copy the value of this attribute
                            // get handles
                            //
                            UBERVECTOR_MEMMOVE(
                                this->attributePtr(this->mpFormat->_getAttributeHandle(*attrib_iter)),
                                el.attributePtr(el.mpFormat->_getAttributeHandle(*attrib_iter)),
                                attrib_iter->size());
                            /*this->attribute(CoreAccess::mpFormat->getAttributeHandle(**attrib_iter))
                                = el.attribute(el.mpFormat->getAttributeHandle(**attrib_iter));*/
                        }
                    }
                }
            }
            return *this;
        }

    protected:

    };
    
};

template <class Traits> class UberVectorT : public Traits
{
public:

    typedef typename Traits::Format Format;
    typedef typename Traits::ElementHandle ElementHandle;
    typedef typename Traits::ConstElementHandle ConstElementHandle;
    typedef typename Traits::Attribute Attribute;

    //--------------------------------------------------------------------------------

    class iterator
    {
    public:
        iterator(Pointer ptr, const Format& fmt)
            : mAddr(ptr), mFormat(fmt) /*, mStride(fmt.mSize)*/
        {
        }

        inline iterator& operator++(void)
        {
            mAddr += mFormat.mSize;
            return *this;
        }

        inline iterator& operator+=(int inc)
        {
            mAddr += inc * mFormat.mSize;
            return *this;
        }

        inline bool operator !=(iterator iter)
        {
            return mAddr!=iter.mAddr;
        }

        ElementHandle& operator*()
        {
            //return *(reinterpret_cast<typename Core::PointInterface*>(this));
            return *((ElementHandle*)(this));
        }

        ElementHandle* operator->()
        {
            //return reinterpret_cast<typename Core::PointInterface*>(this);
            return (ElementHandle*)(this);
        }

    protected:
        Pointer mAddr;
        const Format& mFormat;
    };

    //--------------------------------------------------------------------------------

    class const_iterator
    {
    public:
        const_iterator(ConstPointer ptr, const Format& fmt)
            : mAddr(ptr), mFormat(fmt) /*, mStride(fmt.mSize)*/
        {
        }

        inline const_iterator& operator++(void)
        {
            mAddr += mFormat.mSize;
            return *this;
        }

        inline const_iterator& operator+=(int inc)
        {
            mAddr += inc * mFormat.mSize;
            return *this;
        }

        inline bool operator !=(const_iterator iter)
        {
            return mAddr!=iter.mAddr;
        }

        ConstElementHandle& operator*()
        {
            //return *(reinterpret_cast<typename Core::PointInterface*>(this));
            return *((ConstElementHandle*)(this));
        }

        ConstElementHandle* operator->()
        {
            //return reinterpret_cast<typename Core::PointInterface*>(this);
            return (ConstElementHandle*)(this);
        }

    protected:
        ConstPointer mAddr;
        const Format& mFormat;
    };

  //--------------------------------------------------------------------------------

public:

    UberVectorT(const typename Traits::AttributeList& as)
        : mFormat(as)
    {
        mIncrementSize = ArithPtr(mFormat.size()) * ArithPtr(10000);
        #ifdef UBERVECTOR_USE_MALLOC
        mFirst = mLast = (Pointer)malloc(mIncrementSize);
        #else
        mFirst = mLast = new ubyte[mIncrementSize];
        #endif
        mLastReserved = Pointer(ArithPtr(mFirst) + mIncrementSize);
    }
    
    UberVectorT(const Format& f)
        : mFormat(f)
    {
        mIncrementSize = ArithPtr(mFormat.size()) * ArithPtr(10000);
        #ifdef UBERVECTOR_USE_MALLOC
        mFirst = mLast = (Pointer)malloc(mIncrementSize);
        #else
        mFirst = mLast = new ubyte[mIncrementSize];
        #endif
        mLastReserved = Pointer(ArithPtr(mFirst) + mIncrementSize);
    }

    virtual ~UberVectorT()
    {
        #ifdef UBERVECTOR_USE_MALLOC
        free(mFirst);
        #else
        delete[] mFirst;
        #endif
    }

    private:
    UberVectorT(const UberVectorT& src)
    {}
    public:

    /** If n is less than or equal to capacity(), this call has no effect.
        Otherwise, it is a request for allocation of additional memory. If the request is successful, then capacity() is greater than or equal to n; otherwise, capacity() is unchanged. In either case, size() is unchanged.
    */
    inline void reserve(uint n)
    {
        if (n>capacity())
        {
            #ifdef UBERVECTOR_USE_MALLOC
            Pointer newFirst = (Pointer)malloc(ArithPtr(n * mFormat.size()));
            #else
            Pointer newFirst = new ubyte[ArithPtr(n * mFormat.size())];
            #endif
            memcpy(newFirst, mFirst, ArithPtr(mLast)-ArithPtr(mFirst));
            #ifdef UBERVECTOR_USE_MALLOC
            free(mFirst);
            #else
            delete[] mFirst;
            #endif
            mLast = (Pointer)(ArithPtr(newFirst) + ArithPtr(mLast) - ArithPtr(mFirst));
            mLastReserved = Pointer(ArithPtr(mLast) + ArithPtr(mFormat.size()*n));
            mFirst = newFirst;
        }
    }

    /** Resize the container.
        Note that internally, this method allocates n+1 elements.
    */
    inline void resize(uint n)
    {
        n++; // allocate one more element to be able to perform some operation
        #ifdef UBERVECTOR_USE_MALLOC
        Pointer newFirst = (Pointer)malloc(n * mFormat.size());
        #else
        Pointer newFirst = new ubyte[n * mFormat.size()];
        #endif
        memcpy(newFirst, mFirst, Math::Min(ArithPtr(n * mFormat.size()),ArithPtr(mLast)-ArithPtr(mFirst)));
        #ifdef UBERVECTOR_USE_MALLOC
        free(mFirst);
        #else
        delete[] mFirst;
        #endif
        mLastReserved = Pointer(ArithPtr(newFirst) + ArithPtr(n * mFormat.size()));
        mLast = Pointer(ArithPtr(newFirst) + ArithPtr((n-1) * mFormat.size()));
        mFirst = newFirst;
    }
    
    inline void clear(void)
    {
        mLast = mFirst;
    }
    
    inline void clear(const typename Traits::AttributeList& as)
    {
        mFormat.clear(as);
        #ifdef UBERVECTOR_USE_MALLOC
        free(mFirst);
        #else
        delete[] mFirst;
        #endif
        mIncrementSize = ArithPtr(mFormat.size()) * ArithPtr(10000);
        #ifdef UBERVECTOR_USE_MALLOC
        mFirst = mLast = (Pointer)malloc(mIncrementSize);
        #else
        mFirst = mLast = new ubyte[mIncrementSize];
        #endif
        mLastReserved = Pointer(ArithPtr(mFirst) + ArithPtr(mIncrementSize));
    }

    /** Add one new element at the end of the container and the return this element.
        \warning This operation may remove the consistency/validity of existing iterators.
    */
    inline ElementHandle append(void)
    {
        mLast = Pointer(ArithPtr(mLast)+ArithPtr(mFormat.size()));
        if(mLast>=mLastReserved)
        {
            mIncrementSize = ArithPtr(mFormat.size()) * ArithPtr(capacity()/2);
            //LOG_DEBUG("increase buffer size, old capacity =  " + QString::toString((int)capacity()) );
            #ifdef UBERVECTOR_USE_MALLOC
            Pointer newFirst = (Pointer)malloc((ArithPtr)mLastReserved-(ArithPtr)mFirst + mIncrementSize);
            #else
            Pointer newFirst = new ubyte[ArithPtr(mLastReserved)-ArithPtr(mFirst) + mIncrementSize];
            #endif
            memcpy(newFirst, mFirst, ArithPtr(mLastReserved)-ArithPtr(mFirst));
            #ifdef UBERVECTOR_USE_MALLOC
            free(mFirst);
            #else
            delete[] mFirst;
            #endif
            mLast = Pointer(ArithPtr(newFirst) + ArithPtr(mLast) - ArithPtr(mFirst));
            mLastReserved = Pointer(ArithPtr(newFirst) + ArithPtr(mLastReserved) - ArithPtr(mFirst) + mIncrementSize);
            mFirst = newFirst;
            //LOG_DEBUG("increase buffer size : new capacity = " + QString::toString((int)capacity()) );
        }
        return ElementHandle(Pointer(ArithPtr(mLast)-ArithPtr(mFormat.size())), mFormat);
    }
    
    inline void erase(uint i)
    {
        UBERVECTOR_MEMMOVE( Pointer(ArithPtr(mFirst)+ArithPtr(i*mFormat.size())),
            Pointer(ArithPtr(mFirst)+ArithPtr((i+1)*mFormat.size())),
            ArithPtr((size()-(i+1))*mFormat.size()) );

        Pointer tempLast = Pointer(ArithPtr(mLast) - ArithPtr(mFormat.size()));
        if(tempLast>=mFirst)
        {
            mLast=tempLast;
        }
    }
    
    /** Remove the elements indexed by the set indexes
    */
    template <class IndexSetT> void erase(IndexSetT indexes)
    {
        std::sort(indexes.begin(),indexes.end());
        eraseSortedSet(indexes);
    }
    
    /** Remove the elements indexed by the set indexes
        \warning the set of indexes must be ordered
    */
    template <class IndexSetT> void eraseSortedSet(const IndexSetT& indexes)
    {
        //assert(__gnu_cxx::is_sorted(indexes.begin(),indexes.end()));
        
        typename IndexSetT::const_iterator it = indexes.begin();
        int prevId = *it;
        int moveTo = prevId;
        int countDeleted = 0;
        ++it;
        for ( ; it!=indexes.end() ; ++it)
        {
            int id = *it;
            
            if (id>=size())
                break;
            
            if (id!=prevId)
            {
                ++countDeleted;
                int nb = id - prevId - 1;
                if (nb>0)
                {
                    UBERVECTOR_MEMMOVE( Pointer(ArithPtr(mFirst)+ArithPtr(((moveTo)*mFormat.size()))),
                            Pointer(ArithPtr(mFirst)+ArithPtr((prevId+1)*mFormat.size())),
                            ArithPtr(nb*mFormat.size()) );
                    moveTo += nb;
                }
                prevId = id;
            }
        }
        // move the last chunk
        {
            int nb = size() - prevId - 1;
            if (nb>0)
            {
                UBERVECTOR_MEMMOVE( Pointer(ArithPtr(mFirst)+ArithPtr((moveTo)*mFormat.size())),
                        Pointer(ArithPtr(mFirst)+ArithPtr((prevId+1)*mFormat.size())),
                        ArithPtr(nb*mFormat.size()) );
            }
        }
        
        mLast = Pointer(ArithPtr(mLast)-ArithPtr(countDeleted*mFormat.size()));
    }

    /** Remove the last element of the container.
    */
    inline void popBack()
    {
        /// \todo fixme
        Pointer tempLast = Pointer(ArithPtr(mLast) - ArithPtr(mFormat.size()));
        if(tempLast>=mFirst)
        {
            mLast=tempLast;
        }
        else
        {
//             LOG_ERROR("ContainerT::popBack : no element to remove");
        }
    }

    /** Remove the last element of the container. (STL like)
    */
    inline void pop_back()
    {
        /// \todo fixme
        Pointer tempLast = Pointer(ArithPtr(mLast) - ArithPtr(mFormat.size()));
        if(tempLast>=mFirst)
        {
            mLast=tempLast;
        }
        else
        {
//             LOG_ERROR("ContainerT::pop_back : no element to remove");
        }
    }

    /** Add one new element at the end of the container (with copy) and the return this element. (STL like)
        \warning This operation may remove the consistency/validity of existing iterators.
    */
    inline ElementHandle push_back(ConstElementHandle pt)
    {
        /// \todo fixme
        //LOG_ERROR("Have to be fixed");
        return append().copyFrom(pt);
    }


    /** Return the number of elements.
    */
    inline uint size(void) const
    {
        return (ArithPtr(mLast)-ArithPtr(mFirst))/ArithPtr(mFormat.size());
    }

    /** Return the number of elements plus the numeber of elements that can be stored into the pre-allocated parts.
    */
    inline uint capacity(void) const
    {
        return (ArithPtr(mLastReserved)-ArithPtr(mFirst))/ArithPtr(mFormat.size());
    }

    /** Free the pre-allocated memory parts. This is equivalent to reserve(0);
    */
    void squeeze(void)
    {
        #ifdef UBERVECTOR_USE_MALLOC
        Pointer newFirst = (Pointer)malloc( (ArithPtr)(size() * mFormat.size()) );
        #else
        Pointer newFirst = new ubyte[ArithPtr(size() * mFormat.size())];
        #endif
        memcpy(newFirst, mFirst, ArithPtr(mLast)-ArithPtr(mFirst));
        #ifdef UBERVECTOR_USE_MALLOC
        free(mFirst);
        #else
        delete[] mFirst;
        #endif
        mLast = Pointer(ArithPtr(newFirst) + ArithPtr(mLast) - ArithPtr(mFirst));
        mLastReserved = mLast;
        mFirst = newFirst;
    }

    /** Get the address of the begin of the data.
    */
    Pointer data(uint i = 0)
    {
        return Pointer(ArithPtr(mFirst)+ArithPtr(i*mFormat.size()));
    }

    /** Get an iterator to the fist element.
    */
    inline iterator begin()
    {
        return iterator(mFirst, mFormat);
    }

    /** Get the iterator following the last element.
    */
    inline iterator end()
    {
        return iterator(mLast, mFormat);
    }

    /** Get an iterator to the fist element. (for read-only use)
    */
    inline const_iterator begin() const
    {
        return const_iterator(mFirst, mFormat);
    }

    /** Get the iterator following the last element. (for read-only use)
    */
    inline const_iterator end() const
    {
        return const_iterator(mLast, mFormat);
    }

    /** Get the last element.
    */
    inline ElementHandle back(void)
    {
        return ElementHandle(mLast-mFormat.size(), mFormat);
    }

    /** Get the last element. (for read-only use)
    */
    inline ConstElementHandle back(void) const
    {
        return ElementHandle(Pointer(ArithPtr(mLast)-ArithPtr(mFormat.size())), mFormat);
    }

    /** Get the element number i of the array;.
    */
    inline ElementHandle at(uint i)
    {
        return ElementHandle(Pointer(ArithPtr(mFirst)+ArithPtr(i*mFormat.size())), mFormat);
    }

    /** Get the element number i of the array.
    */
    inline ElementHandle operator[](uint i)
    {
        //return at(i);
        return ElementHandle(Pointer(ArithPtr(mFirst)+ArithPtr(i*mFormat.size())), mFormat);
    }

    /** Get the element number i of the array. (for read-only use)
    */
    inline ConstElementHandle at(uint i) const
    {
        return ElementHandle(Pointer(ArithPtr(mFirst)+ArithPtr(i*mFormat.size())), mFormat);
    }

    /** Get the element number i of the array. (for read-only use)
    */
    inline ConstElementHandle operator[](uint i) const
    {
        //return at(i);
        return ElementHandle(Pointer(ArithPtr(mFirst)+ArithPtr(i*mFormat.size())), mFormat);
    }

    /** Get the index of an element in the array.
    */
    inline int getIndexOf(const ElementHandle& el)
    {
        return (ArithPtr(el._getAddr()) - ArithPtr(mFirst)) / ArithPtr(mFormat.size());
    }
    
    /** swap two elements
    */
    inline void swap(uint i0, uint i1)
    {
        if (mLast==mLastReserved)
        {
            reserve(size()+1);
        }
        at(size()).copyFrom(at(i0));
        at(i0).copyFrom(at(i1));
        at(i1).copyFrom(at(size()));
    }

    /** Test if the elements of the array have the queryied attribute.
        \param p the queried attribute
    */
    bool hasAttribute(const Attribute& p) const
    {
        return mFormat.hasAttribute(p);
    }

    void removeAttribute(const Attribute& p);


    /**
        \todo insert the new free space at correct position in the struct according to the format
              and not simply at the end of the struct
    */
    //template<class T,uint T_Id> void addAttribute(typename Traits::template MetaAttributeT<T,T_Id> & a)
//     template< template<class T,uint T_Id> class _MetaAttributeT >
//     void addAttribute(_MetaAttributeT<class T,T_Id>& a)
    template< class _MetaAttributeT >
    void addAttribute(_MetaAttributeT& a)
    {
//         LOG_ERROR("Have to be fixed !");

        ArithPtr oldStride = ArithPtr(mFormat.size());
        ArithPtr nofReservedElement = (ArithPtr(mLastReserved)-ArithPtr(mFirst))/oldStride;
        ArithPtr nofElement = (ArithPtr(mLast)-ArithPtr(mFirst))/oldStride;
        mFormat.addAttribute(a);
        ArithPtr newStride = ArithPtr(mFormat.size());

        // resize the vector and update data stride
        #ifdef UBERVECTOR_USE_MALLOC
        Pointer newFirst = (Pointer)malloc(nofReservedElement * ArithPtr(mFormat.size()));
        #else
        Pointer newFirst = new ubyte[nofReservedElement * ArithPtr(mFormat.size())];
        #endif
        // copy with stride
        for(ArithPtr i=0 ; i<nofElement ; ++i)
        {
            memcpy(Pointer(ArithPtr(newFirst) + i*newStride), Pointer(ArithPtr(mFirst) + i*oldStride), oldStride);
        }
        #ifdef UBERVECTOR_USE_MALLOC
        free(mFirst);
        #else
        delete[] mFirst;
        #endif
        mLast = Pointer(ArithPtr(newFirst) + newStride * nofElement);
        mLastReserved = Pointer(ArithPtr(newFirst) + newStride * nofReservedElement);
        mFirst = newFirst;
    }

    template<class T, uint T_Id>
    typename Traits::template MetaAttributeT<T,T_Id>::Handle& getAttributeHandle(typename Traits::template MetaAttributeT<T,T_Id>& p)
    {
        return mFormat.getAttributeHandle(p);
    }

    /** Get the formats of the elements stored by the container.
    */
    const Format& getFormat(void) const
    {
        return mFormat;
    }

protected:

    Format mFormat;
    Pointer mFirst, mLast, mLastReserved;
    ArithPtr mIncrementSize;

};

}

#endif



