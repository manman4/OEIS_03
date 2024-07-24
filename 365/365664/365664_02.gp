a(n) = my(x='x+O('x^(n+10))); polcoef(sum(i=1, n\4, sum(j=i+1, n, sum(k=j+1, n, sum(l=k+1, n, x^(i+j+k+l)/( (1-x^i)*(1-x^j)*(1-x^k)*(1-x^l) )^2 )))), n);
for(n=10, 49, print1(a(n),", "))

\\ b(n) = (5*sigma_7(n) - (126*n-441)*sigma_5(n) + (756*n^2-4410*n+4935)*sigma_3(n) - (840*n^3-5880*n^2+9870*n-3229)*sigma(n))/967680
b(n) = (5*sigma(n, 7) - (126*n-441)*sigma(n, 5) + (756*n^2-4410*n+4935)*sigma(n, 3) - (840*n^3-5880*n^2+9870*n-3229)*sigma(n))/967680;
for(n=10, 49, print1(a(n)-b(n),", "))
