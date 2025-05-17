c(n) = {
    if(n == 0, return(1));
    my(sum = 0);
    for(i=0, n,
        for(j=0, n,
            for(k=0, n,
                if(i + j + k == n,
                    sum += abs(stirling(i+n-1, n-1, 2) * stirling(j+n-1, n-1, 2) * stirling(k+n-1, n-1, 2));
                )
            )
        )
    );
    return(sum);
};

for(n=0, 20, print1(c(n),", "));
