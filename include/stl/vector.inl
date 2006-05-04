#include "vector"


namespace std
{


// -----------------------------------------------------------------------------
template <class T>
vector<T>::vector()
 : iUsed_(0)
{
}

// -----------------------------------------------------------------------------
template <class T>
vector<T>::~vector()
{
}

// -----------------------------------------------------------------------------
template <class T>
void
vector<T>::push_back(const_reference t)
{
  if(iUsed_ < MAX_VECTOR_SIZE)
  {
    vec_[iUsed_] = t;
    iUsed_++;
  }
}

// -----------------------------------------------------------------------------
template <class T>
const T & vector<T>::operator[](size_type n) const
{
  if(n < iUsed_)
    return vec_[n];
  else
    return vec_[0];  // FIXME: Should throw error, but we don't have exceptions
}

// -----------------------------------------------------------------------------
template <class T>
T & vector<T>::operator[](size_type n)
{
  if(n < iUsed_)
    return vec_[n];
  else
    return vec_[0];  // FIXME: Should throw error, but we don't have exceptions
}


}
