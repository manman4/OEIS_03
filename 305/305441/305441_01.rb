require 'prime'

# m���ȉ������o��
def mul(f_ary, b_ary, m)
  s1, s2 = f_ary.size, b_ary.size
  ary = Array.new(s1 + s2 - 1, 0)
  (0..s1 - 1).each{|i|
    (0..s2 - 1).each{|j|
      ary[i + j] += f_ary[i] * b_ary[j]
    }
  }
  ary[0..m]
end

def A(n)
  ary = [1] + [0] * n
  (1..n).each{|i|
    si = i * (3 * i - 1) / 2
    break if si > n
    b_ary = Array.new(si + 1, 0)
    b_ary[0], b_ary[-1] = 1, -1
    ary = mul(ary, b_ary, n)
  }
  ary
end

m = 11000
a = A(m)
ary = (0..n).select{|i| a[i] == 0}
(1..ary.size).each{|i|
  print i
  print ' '
  puts ary[i - 1]
}
