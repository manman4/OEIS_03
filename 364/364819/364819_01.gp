a(n) = if( n<0, 0, n! * polcoeff( exp( x + x^2 / 2 + x * O(x^n)), n));

b(n) = sum(k=1, n\4, (n-2*k)! * a(n-4*k) / (k! * (n-4*k)!) );
for(n=2, 28, print1(b(n), ", "))