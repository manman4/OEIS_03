M=19;

a(n) = round(((10+sqrt(10))^n-(10-sqrt(10))^n)/(2*sqrt(10)));
for(n=0, M, print1(a(n), ", "));

a(n) = polcoef(lift(Mod(x, (x-10)^2-10)^n), 1);
for(n=0, M, print1(a(n), ", "));
