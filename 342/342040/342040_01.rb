def A(n)
  ary = []
  cnt = 1
  while ary.size < n
    str = cnt.to_s(2)
    if str.size % 2 == 1 && str == str.reverse
      ary << str.to_i
    end
    cnt += 1
  end
  ary
end


p A(30)