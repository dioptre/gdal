/******************************************************************************
 * $Id$
 *
 * Project:  CPL - Common Portability Library
 * Author:   Marek Brudka, mbrudka@aster.pl
 * Purpose:  
 *  o provides a template similar to boost::intrusive_ptr
 *    to facilate intrusive reference counting 
 *
 ******************************************************************************
 * Copyright (c) 2005, Marek Brudka
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 * $Log$
 * Revision 1.1.2.1  2005/06/23 12:51:05  mbrudka
 * Helper class for reference counted GDAL objects.
 *
 */

#ifndef _CPL_INTRUSIVE_PTR_H_INCLUDED_
#define _CPL_INTRUSIVE_PTR_H_INCLUDED_

/** Simple template to automate reference counting.
 * 
 * This template is responsible to automatic reference counter changes.
 * It is simplified version of boost::intrusive_ptr. The templated 
 * parameter T should conform to Refcountable concept i.e. it should
 * provide two methods: 
 * - T::Reference(), which increases the reference counter of T instance
 * - T::Derefence(), which decreases  the reference counter of T instance 
 *   and deallocates T if necessary.
 *
 * CPLIntrusivePtr<T> is not thread safe, namely one cannot modify
 * the same instance of CPLIntrusivePtr<T> in various threads. 
 * Nevertheless, if T::Reference, T::Dereference methods 
 * are thread safe, namely the counter is guarded and T::~T is thread safe, 
 * then one can use different instances of  * CPLIntrusivePtr<T> to
 * manage the life cycle of same instace of T in different threads.
 *
 * Typical usage
 * @code
 typedef CPLIntrusivePtr< OGRSpatialReference > OGRSpatialReferenceIVar;
 void f( OGRGeometry &p_geometry  )
 {
    OGRSpatialReferenceIVar sr( p_geometry.GetSpatialReference() );
    if ( sr.get() )
      ; //..
 } // here sr is dereferenced.
 * @endcode
 */
template< class T>
class CPLIntrusivePtr
{
public:
   /** Starts managing the lifecycle of T.
    * @param po T instance to be managed.
    * Method start managing the lifecycle of po, by calling po->Reference.
    * After this initialization it is impossible to stop the managing
    * of po lifecycle, because intentionally there is no release method.
    */
    CPLIntrusivePtr( T *po = 0):
        poObj( po )
    {
        if ( poObj )
            poObj->Reference();
    }

   /** Starts sharing of T instance managed by rhs.
    * @param rhs The intrusive pointer to T instance.
    * Method start sharing the object with rhs, by calling po->Reference.
    */
    CPLIntrusivePtr( const CPLIntrusivePtr< T >  &rhs):
        poObj( rhs.poObj )
    {
        if ( poObj )
            poObj->Reference();
    }

   /** Dereferences the managed T instance.
    */
    virtual ~CPLIntrusivePtr( )
    {
        if ( poObj )
          poObj->Dereference();
    }

   /** Starts sharing of T instance managed by rhs.
    * @param rhs The intrusive pointer to T instance.
    * Method start sharing the object with rhs, by calling po->Reference, 
    * and dereference the previously managed object.
    */
    CPLIntrusivePtr< T > &operator=( const CPLIntrusivePtr< T >  &rhs)
    {
        if ( this != &rhs )
        {
            if ( rhs.poObj )
                rhs.poObj->Reference();
            if ( poObj )
                poObj->Dereference();
            poObj = rhs.poObj;
        }
        return *this;
    }

   /** Starts managing of T instance.
    * @param po T instance to be managed.
    * Method start managing the lifecycle of po, by calling po->Reference.
    * After this assignement it is impossible to stop the managing
    * of po lifecycle, because intentionally there is no release method.
    */
    CPLIntrusivePtr< T > &operator=(T * rhs)
    {
        if ( rhs )
            rhs->Reference();
        if ( poObj )
            poObj->Dereference();
        poObj = rhs;
        return *this;
    }

   /** Accessor to managed T instance.
    * @return Pointer to managed T instance or 0 if nothing is managed.
    */
    T * get() const
    {
        return poObj;
    }

   /** Accessor to managed T instance.
    * @return Reference to managed T instance or empty reference 
    *         if nothing is managed.
    */
    T & operator*() const
    {
        return *poObj  ; //poObj==NULL? SIGSEGV then
    }

   /** Accessor to managed T instance.
    * @return Reference to managed T instance or empty reference 
    *         if nothing is managed.
    */
    T * operator->() const
    {
        return poObj; //poObj==NULL? SIGSEGV then
    }

private:
    T *poObj; //!< pointer to managed T instance.
};

#endif /* _CPL_INTRUSIVE_PTR_H_INCLUDED_ */

