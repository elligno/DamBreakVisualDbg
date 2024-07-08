#ifndef observer_H
#define observer_H

namespace dbpp { 
	/** Brief Observer design pattern
	*
	*  Provide a single method for client called to update
	*  after a notification (setState).
	*/
	class Observer { 
	public: virtual void update()=0;
	};
} // End of namespace
#endif // Include guard