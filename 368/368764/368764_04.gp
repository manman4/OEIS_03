a368575(n) = n! * sum(k=0, n, binomial(k+3,4) / k!);
for(n=0, 21, print1(a368575(n), ", "));
a(n) = n! + a368575(n);
for(n=0, 21, print1(a(n), ", "));
