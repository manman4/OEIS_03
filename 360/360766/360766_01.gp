M=18;

a(n) = if(n==0, 0, round(((n+sqrt(n))^n-(n-sqrt(n))^n)/(2*sqrt(n))));
for(n=0, M, print1(a(n), ", "));

a(n) = polcoef(lift(Mod(x, (x-n)^2-n)^n), 1);
for(n=0, M, print1(a(n), ", "));
