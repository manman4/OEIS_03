default(parisize, 1200000000)

M=17;

\\ Diagonal of the rational function 1/(1 - (v^3 + w^3 + x^3 + y^3 + z^3 + v*w*x*y*z)).
a(n) = my(x='x+O('x^(n+1)), y='y+O('y^(n+1)), z='z+O('z^(n+1)), w='w+O('w^(n+1)), v='v+O('v^(n+1))); polcoef(polcoef(polcoef(polcoef(polcoef(1/(1-(v^3+w^3+x^3+y^3+z^3+v*w*x*y*z)), n), n), n), n), n);
for(n=0, M, print1(a(n),", "));

