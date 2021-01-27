require 'prime'

@p_ary = [0] + Prime.take(10 ** 7).to_a

def A(n)
  ary = []
  (1..n).each{|i|
    (1..i).each{|j|
      ary << @p_ary[j] * @p_ary[i + 1 - j] - @p_ary[j * (i + 1 - j)]
    }
  }
  ary
end

n = 140
ary = A(n)
(1..ary.size).each{|i|
  j = ary[i - 1]
  break if j.to_s.size > 1000
  print i
  print ' '
  puts j
}
