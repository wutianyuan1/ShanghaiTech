x0=[-0.8;-0.8];
Nsamp=10^4;
d=1/2;


dim_x = length(x0); 
beta = 0.1; 
chain = zeros(dim_x,Nsamp); 
chain(:,1) = x0;

x = x0;

ar = 0;

for n = 1:Nsamp-1
    
    z = x + beta*randn(dim_x,1);
    
    px = postdis(x,d);
    pz = postdis(z,d);
    
    a = min(1,pz/px);
    
   if rand<a  
       x = z;
       ar = ar+1;
   end
   chain(:,n+1)=x;
end
ar = ar/Nsamp
plot(chain(1,:),chain(2,:),'*')    

    

    function p = postdis(x,d)
   
    
    sigma = 0.1;

    G = forwardfun(x); 
    
    x_min = -1*ones(size(x));
    x_max = 1*ones(size(x));
    
    if (x>x_min) & (x<x_max)
    
    p = exp(-0.5*sum((G - d).^2)/sigma^2);
    
    else
        p = 0;
    end
    
    end


    function G = forwardfun(x)
            
           
            G=x'*x;
        end 