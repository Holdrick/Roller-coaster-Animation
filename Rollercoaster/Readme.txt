Taylor Cooper - 10121325

To run: Run make and call ./asg1

Controls:
Arrow keys -> rotate the scene
Left/Right mouse buttons -> zoom in and out
r -> draw/not draw the rails
p -> draw/not draw the plane
o -> orient/not orient the cart
w -> draw/not draw the wire

Description:
	Physics:
		I used the formula speed = sqrt(2g*(H-H)) for calculating speed after the initial hill climb. I applied a 
		minimum speed for the initial hill climb. For the deceleration I used the formula:
		speed = currentSpeed * 0.96. I applied this until the minimum speed was reached.
		
	Math/Algorithms:
		To create the spline curve I took a set of control points and subdivided it using the method discussed
		in tutorial. To determine the position of the cart I used the algorithm/pseudo code outlined in 
		tutorial. For time change I used a constant value instead of basing it off the system clock.
		
		
Sources:
	I got a lot of my graphics code (including shaders) from cpsc 453 with Dr. Alim. I got everything
	else from tutorials and lectures.
		