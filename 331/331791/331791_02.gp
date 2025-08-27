\\ T(n,k) = [x^n] (1+k*x+(k-1)*x^2)^(n+1).
T(n,k) = polcoeff( (1+k*x+(k-1)*x^2)^(n+1), n);
for(n=0, 10, for(k=0, n, print1(T(k,n-k),", ")));

my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ For k != 1, e.g.f. of column k: exp(k*x) * BesselI(1, 2*sqrt(k-1)*x) / sqrt(k-1), with offset 1.
S(n,k) = if(k==1, T(n,1), my(x='x+O('x^(n+2))); round( (n+1)! * polcoef(exp(k*x) * my_besseli(1, 2*sqrt(k-1)*x) / sqrt(k-1), n+1) ));
for(n=0, 20, for(k=0, n, print1(T(k,n-k)-S(k,n-k),", ")));