a(n) = (n-1)*(n-2)*sigma(n);
for(n=1, 42, print1(a(n), ", "))

\\ a(n) >= 8 * A002127(n) and the equal sign only holds if n is 1 or prime.
a002127(n) = ( sigma( n, 3) - (2*n - 1) * sigma(n) ) / 8;
for(n=1, 100, print1(a(n) - 8 * a002127(n), ", "))
for(n=1, 100, if( a(n) == 8 * a002127(n), print1(n, ", ")) )
