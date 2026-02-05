# a(2*n) = a(n) and a(2*n+1) = Sum_{i,j,k>=0 and i+j+k=n} a(i) * a(j) * a(k) with a(0)=1.
def A393195(n)
  ary = [1]
  (1..n).each{|i|
    m = i / 2
    if i.even?
      ary << ary[m]
    else
      s = 0
      (0..m).each{|j|
        (0..m - j).each{|k|
          s += ary[j] * ary[k] * ary[m - j - k]
        }
      }
      ary << s
    end
  }
  ary
end

p A393195(60)

# n = 1000
# ary = a(n)
# (0..n).each{|i| 
#   j = ary[i]
#   break if j.to_s.size > 1000
#   print i
#   print ' '
#   puts j
# }