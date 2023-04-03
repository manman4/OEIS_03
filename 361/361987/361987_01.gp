a(n) = {
    if(n==1, 
        1,
        n^2 * sumdiv(n, d, if(d<n, (-1)^(n/d) * a(d) / d^2))
    )
};

for(n=1, 50, print1(a(n),", "))
