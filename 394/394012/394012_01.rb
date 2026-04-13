# T(1,0) = 1;
# T(n,k) = s(k) * T(n-1,k+1) + t_{n-1}(k) * T(n-1,k) + u_{n-1}(k) * T(n-1,k-1);
# T(n,k) = 0 if k < 0 or k >= n, 
# where s(k) = 2*(k+1)*(2*k+1), t_n(k) = 4*(4*n*k + 2*k^2 + n + k), u_n(k) = 2*(4*n + 2*k + 1)*(2*n + k - 1).

def compute_all_d_values(limit)
  # d[j] は現在の m における d_{m, j}
  # 初期条件 d_{1, 0} = 1
  a = [1]
  
  (1..limit).each do |m|
    # m ステップ目の全係数 d_{m, j} を表示
    # puts "--- m = #{m} ---"
    a.each_with_index do |val, j|
      # puts "d(#{m}, #{j}) = #{val}"
      print("#{val}, ")
    end
    # puts "a(#{m}) = #{d[0]}" # これが求める一般項
    # puts ""

    next_a = Array.new(m + 1, 0)
    (0..m).each do |k|
      # 1. α係数: d_{m, k+1} からの寄与
      alpha_term = (k + 1 < a.size) ? 2 * (k + 1) * (2 * k + 1) * a[k + 1] : 0
      
      # 2. β係数: d_{m, k} からの寄与
      beta_term = (k < a.size) ? 4 * (4 * m * k + 2 * k**2 + m + k) * a[k] : 0
      
      # 3. γ係数: d_{m, k-1} からの寄与
      gamma_term = (k > 0) ? 2 * (4 * m + 2 * k + 1) * (2 * m + k - 1) * a[k - 1] : 0
      
      next_a[k] = alpha_term + beta_term + gamma_term
    end
    a = next_a
  end
end

compute_all_d_values(7)