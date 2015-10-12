#pragma once
#include "Mesh/Flip.h"

namespace Egg { namespace Mesh
{
	class Multi
	{
		typedef std::vector<Egg::Mesh::Flip::P> SubmeshVector;
		SubmeshVector submeshes;

		Multi(){}
	public:
		typedef boost::shared_ptr<Multi> P;
		static Multi::P create() { return P(new Multi());}

		void add(Egg::Mesh::Flip::P flip)
		{
			submeshes.push_back(flip);
		}

		Egg::Mesh::Flip::P getSubmesh(unsigned int index)
		{
			//TODO error check
			return submeshes.at(index);
		}

		void draw(ID3D11DeviceContext* context, Mien mien)
		{
			SubmeshVector::iterator i = submeshes.begin();
			SubmeshVector::iterator e = submeshes.end();
			while(i != e)
			{
				(*i)->draw(context, mien);
				i++;
			}
		}
	};

}} // namespace Egg::Mesh
