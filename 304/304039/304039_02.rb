# mŽŸˆÈ‰º‚ðŽæ‚èo‚·
def mul(f_ary, b_ary, m)
  s1, s2 = f_ary.size, b_ary.size
  ary = Array.new(s1 + s2 - 1, 0)
  (0..s1 - 1).each{|i|
    (0..s2 - 1).each{|j|
      k = b_ary[j]
      ary[i + j] += f_ary[i] * k if k > 0
    }
  }
  ary[0..m]
end

def f(n)
  return 1 if n < 2
  (1..n).inject(:*)
end


def A(n)
  ary = [1]
  (1..n).each{|i|
p i
    j = f(i)
    break if j > n
    a = Array.new(n + 1, 0)
    (0..[n / j, i + 1].min).each{|k| a[k * j] = 1}
    ary = mul(ary, a, n)
  }
  ary
end

n = 5000
p ary = A(n)
p (1..ary.max).map{|i| ary.index(i)}
