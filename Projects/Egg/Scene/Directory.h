#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <map>

namespace Egg { namespace Scene
{
	/// Template class for associative containers mapping string identifiers to objects referenced by shared pointers.
	template<class _Ty>
	class Directory : private std::map<std::string, boost::shared_ptr<_Ty> >
	{
	public:
	using std::map<std::string, boost::shared_ptr<_Ty> >::begin;
	using std::map<std::string, boost::shared_ptr<_Ty> >::end;
	using std::map<std::string, boost::shared_ptr<_Ty> >::operator[];
	using std::map<std::string, boost::shared_ptr<_Ty> >::iterator;
	using std::map<std::string, boost::shared_ptr<_Ty> >::find;
	using std::map<std::string, boost::shared_ptr<_Ty> >::size;
	using std::map<std::string, boost::shared_ptr<_Ty> >::clear;
	using std::map<std::string, boost::shared_ptr<_Ty> >::insert;
	using std::map<std::string, boost::shared_ptr<_Ty> >::erase;
	using std::map<std::string, boost::shared_ptr<_Ty> >::at;
	};

	/// Template class for associative containers mapping string identifiers to objects referenced by weak pointers.
	template<class _Ty>
	class WeakDirectory : private std::map<std::string, boost::weak_ptr<_Ty> >
	{
	public:
	using std::map<std::string, boost::weak_ptr<_Ty> >::begin;
	using std::map<std::string, boost::weak_ptr<_Ty> >::end;
	using std::map<std::string, boost::weak_ptr<_Ty> >::operator[];
	using std::map<std::string, boost::weak_ptr<_Ty> >::iterator;
	using std::map<std::string, boost::weak_ptr<_Ty> >::find;
	using std::map<std::string, boost::weak_ptr<_Ty> >::size;
	};

	/// Template class for associative containers mapping string identifiers to COM object pointers.
	template<class _Ty>
	class DxResourceDirectory : private std::map<std::string, _Ty*>
	{
	public:
		using std::map<std::string, _Ty*>::begin;
		using std::map<std::string, _Ty*>::end;
		using std::map<std::string, _Ty*>::operator[];
		using std::map<std::string, _Ty*>::iterator;
		using std::map<std::string, _Ty*>::find;
		using std::map<std::string, _Ty*>::size;

		void releaseAll()
		{
			iterator i = begin();
			iterator e = end();
			while(i != e)
			{
				if(i->second)
					i->second->Release();	
				i++;
			}
			clear();
		}

		~DxResourceDirectory()
		{
			releaseAll();
		}
	};

}}