#ifndef ERROR_CONTROLLER_H
#define ERROR_CONTROLLER_H

class error_controller
{
	public:
		error_controller();
		virtual ~error_controller();

		virtual double step(double error) = 0;
};

#endif // ERROR_CONTROLLER_H
