def C(n, r)
  r = [r, n - r].min
  return 1 if r == 0
  return n if r == 1
  numerator = (n - r + 1..n).to_a
  denominator = (1..r).to_a
  (2..r).each{|p|
    pivot = denominator[p - 1]
    if pivot > 1
      offset = (n - r) % p
      (p - 1).step(r - 1, p){|k|
        numerator[k - offset] /= pivot
        denominator[k] /= pivot
      }
    end
  }
  result = 1
  (0..r - 1).each{|k|
    result *= numerator[k] if numerator[k] > 1
  }
  return result
end

def mul(f_ary, b_ary)
  s1, s2 = f_ary.size, b_ary.size
  ary = Array.new(s1 + s2 - 1, 0)
  (0..s1 - 1).each{|i|
    (0..s2 - 1).each{|j|
      ary[i + j] += f_ary[i] * b_ary[j]
    }
  }
  ary
end

def f(a)
  ary = [1]
  a.each{|k|
    ary1 = [0]
    (1..k).each{|j|
      ary1 << C(k - 1, k - j) / (1..j).inject(:*).to_r
    }
    ary = mul(ary, ary1)
  }
  s = 0
  (1..ary.size - 1).each{|p|
    s += (1..p).inject(:*) * ary[p]
  }
  s.to_i
end

def g(k, n)
  [1] + (1..n).map{|i| f((1..i).map{k})}
end

def F(n)
  return 1 if n < 2
  (1..n).inject(:*)
end

n = 50
(0..n).each{|i|
  j = g(i, i)[-1] / F(i)
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
