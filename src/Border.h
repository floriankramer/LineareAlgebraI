#ifndef BORDER_H_
#define BORDER_H_

namespace ant{


	class Border{

		public:
			Border(int startX, int startY, int stopX, int stopY);
			Border(int id, int startX, int startY, int stopX, int stopY);
			virtual ~Border();
			void render();
			bool intersectsWith(Border *b);
			int *getIntersectionWith(Border *b);
			int getX1();
			int getY1();
			int getX2();
			int getY2();
			void setX1(int x1);
			void setX2(int x2);
			void setY1(int y1);
			void setY2(int y2);
			int getId();

		private:
			int id;
			int x1, y1, x2, y2; 
		

	};

}
#endif
