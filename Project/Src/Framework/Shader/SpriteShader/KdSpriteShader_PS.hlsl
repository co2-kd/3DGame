#include "inc_KdSpriteShader.hlsli"

// テクスチャ
Texture2D g_inputTex : register(t0);
// サンプラ
SamplerState g_ss : register(s0);


//============================================
// 2D描画 ピクセルシェーダ
//============================================
float4 main(VSOutput In) : SV_Target0
{

	// テクスチャ色取得
	float4 texCol = g_inputTex.Sample(g_ss, In.UV);
	//アルファテスト
	if (texCol.a < 0.1f) discard;
	
    //円形描画用
    if (g_angle > 0.f)
    {
        //--------------------------------------------
        //描画しない条件に当てはまるか
        //--------------------------------------------

        //1.UVの値を考えやすくする
        //画像の中心が(0.5,0.5)だと難しいので
        //中心(0,0)にするために0.5を引く
        float uvX = (In.UV.x - 0.5f);
        float uvY = (In.UV.y - 0.5f) * -1.f; //上に行くほど数字が大きくなるようにするために-1をかける

        //2.距離からこのピクセルの角度をだす
        //基準となる軸からの距離を求める → atan2で角度を求める
        //-180になる位置が基準になる
        float blue = -1 * uvY;
        float red = -1 * uvX;
        float thisAngle = atan2(red, blue);

        //3.値を修正
        //すべて正になるように180°加える
        thisAngle += 3.141592f;
        //thisAngle = DirectX::XMConvertToDegrees(thisAngle);
        //4.定数バッファの角度と比較
        //定数バッファより小さかったら描画しない
        if (thisAngle < g_angle)
        {
            discard;//ここで終了　以下は読まれない
        }
    }

	// テクスチャ色 * 指定色
	return texCol * g_color;
}
