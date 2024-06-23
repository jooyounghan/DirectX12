# DirectX12 Portfolio

***
## Features
1. DXGI Flip Model
***

## DXGI Flip Model
BitBlit Model과 DXGI Flip Model의 차이는 '해당 어플리케이션이 프레임을 업데이트 한 뒤, DWM(Desktop Window Manager)에 해당 콘텐츠를 어떻게 처리하는가?' 이다.
전자의 경우 DWM으로 해당 컨텐츠를 복사(Bit Block Transfer)하고, 후자의 경우 해당 프레임 버퍼를 DWM와 공유하여 단순히 전달하는 처리를 수행한다. 

D3D12의 경우 DXGI Flip Model만을 지원한다. Swap Chain의 설정을 통해 
```DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL```와 ```DXGI_SWAP_EFFECT_FLIP_DISCARD```의 두 가지 설정을 할 수 있다. 해당 프로젝트에서는 ```DXGI_SWAP_EFFECT_FLIP_DISCARD```를 통한 DXGI Flip Model을 채택하였다.
> 전자의 경우 Swap을 할 경우 이전 백버퍼의 내용이 보장되며, 다음 백버퍼의 순서가 일관적으로 유지되는 반면, 후자의 경우 이전 백버퍼의 내용을 버리며(상황에 따라 재사용할 수 있다) 다음 백버퍼의 경우 GPU에서 사용 가능한 백버퍼를 활용한다.