\\ a(n) = n! * [x^n] Sum_{k>=0} (k+1)^n * log(1+x)^k / k!.
a(n) = my(x='x+O('x^(n+1))); n! * polcoef(sum(k=0, n, (k+1)^n * log(1+x)^k/k!), n);
for(n=0, 20, print1(a(n), ", "));

