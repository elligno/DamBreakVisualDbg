
#ifndef genumber_HPP
#define genumber_HPP

namespace qplot 
{
	// generate a sequence of number 
	// (to be used with "generate_n algorithm")
	template<typename T> class NumSequence 
	{
	public:

		// default ctor
		NumSequence( T aInitialValue, T aStep=0.1) 
			: m_Type(aInitialValue)
		{
			m_Delta = static_cast<T>(aStep);            // step as default value
		}

		// function call
		T operator () () { return m_Type+=m_Delta;}

	private:

		// Value to increment by step
		T m_Type;
		// step increment
		T m_Delta;
	};

} // End of namespace
#endif // Inc;ude guard
