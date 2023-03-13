M=10;

T(n, k) = if(k==0, n==1, round(((k+sqrt(k))^n-(k-sqrt(k))^n)/(2*sqrt(k))));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));

T(n, k) = polcoef(lift(Mod('x, ('x-k)^2-k)^n), 1);
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));