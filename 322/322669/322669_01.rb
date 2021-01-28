require 'prime'

@p_ary = [0] + Prime.each(10 ** 7).to_a

def A(n)
  cnt = 1
  while @p_ary[cnt] + @p_ary[cnt + 1] <= @p_ary[cnt + n - 1]
    cnt += 1
  end
  @p_ary[cnt + n - 1]
end

(3..10000).each{|i|
  print i
  print ' '
  puts A(i)
}
