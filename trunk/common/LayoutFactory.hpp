#ifndef DAMARIS_LAYOUTFACTORY_H
#define DAMARIS_LAYOUTFACTORY_H
	
namespace Damaris {
	
	class LayoutFactory {
	private:
		
	public:
		//static int SERIALIZED_SIZE = 8;
		
		static int serialize(const Layout* layout, int64_t* buffer)
		{
			buffer[0] = (int64_t)(layout->getType());
			buffer[1] = (int64_t)(layout->getDimensions());
			
			int d = layout->getDimensions();
			for(int i = 0; i < d; i++)
			{
				buffer[2*i+2] = (int64_t)(layout->getStartIndex(i));
				buffer[2*i+1+2] = (int64_t)(layout->getEndIndex(i));
			}
			return 0;
		}
		
		static Layout* unserialize(const int64_t* buffer) 
		{
			Layout* layout = NULL;
			basic_type_e type = (basic_type_e)(buffer[0]);
			int32_t dimensions = (int32_t)(buffer[1]);
			
			std::vector<int64_t> extents(2*dimensions,0);
			
			for(int i = 0; i < dimensions; i++)
				extents[i] = buffer[i+2];
			
			layout = new Layout(type,dimensions,extents);
			return layout;
		}
	};
}

#endif