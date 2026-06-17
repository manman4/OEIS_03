def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

# a(1) = 1; a(n) = Sum_{i,j,k,l,m >= 0 and i+j+k+l+m=n-2} (n-1)!/((i+1)!*j!*k!*l!*m!) * a(i+1)*a(j+1)*a(k+1)*a(l+1)*a(m+1). 
def a(n)
  ary = [0, 1]
  (2..n).each{|nn|
    s = 0
    (0..nn-2).each{|i|
      (0..nn-2-i).each{|j|
        (0..nn-2-i-j).each{|k|
          (0..nn-2-i-j-k).each{|l|
            m = nn-2-i-j-k-l
            s += f(nn-1)/((f(i+1)*f(j)*f(k)*f(l)*f(m))) * ary[i+1]*ary[j+1]*ary[k+1]*ary[l+1]*ary[m+1]
          }
        }
      }
    }
    ary << s
  }
  ary
end

p a(20)[1..-1]