object Form1: TForm1
  Left = 196
  Top = 120
  Width = 912
  Height = 606
  Caption = 'muchMos'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Memo1: TMemo
    Left = 0
    Top = 0
    Width = 896
    Height = 547
    Align = alClient
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    Lines.Strings = (
      'Programa de analise nodal modificada para calculo de'
      'ponto quiescente e analise permanente senoidal'
      ' '
      'Autores:'
      'Rebeca Araripe - rebecaararipe@poli.ufrj.br'
      'Renata Baptista - r.baptista@poli.ufrj.br'
      'Vinicius Mesquita - viniciusmesquita@poli.ufrj.br')
    ParentFont = False
    ScrollBars = ssBoth
    TabOrder = 0
  end
  object OpenDialog1: TOpenDialog
    FileName = 'teste.net'
    Filter = 'netlists|*.net'
    Options = [ofHideReadOnly, ofNoChangeDir, ofEnableSizing]
    Left = 632
    Top = 16
  end
  object MainMenu1: TMainMenu
    Left = 88
    Top = 16
    object Arquivo1: TMenuItem
      Caption = 'Arquivo'
      object Abrir1: TMenuItem
        Caption = 'Abrir'
        OnClick = Abrir1Click
      end
    end
    object Opes1: TMenuItem
      Caption = 'Op'#231#245'es Gerais'
      object MostrarparametrosMOS1: TMenuItem
        Caption = 'Mostrar parametros MOS'
        OnClick = MostrarparametrosMOS1Click
      end
      object MostraEstampasPO: TMenuItem
        Caption = 'Mostrar estampas PO'
        OnClick = MostrarEstampasPOClick
      end
      object MostrarEstampasAC1: TMenuItem
        Caption = 'Mostrar estampas AC'
        OnClick = MostrarEstampasAC1Click
      end
      object MostrarConvergencia1: TMenuItem
        Caption = 'Resultados por iteracao'
        OnClick = MostrarConvergencia1Click
      end
      object MostrarErros1: TMenuItem
        Caption = 'Resultados e erros por iteracao'
        OnClick = MostrarErros1Click
      end
      object SalvaParamTrans1: TMenuItem
        Caption = 'Arquivo com parametros transistor'
        OnClick = SalvaParamTrans1Click
      end
      object SalvaPO1: TMenuItem
        Caption = 'Arquivo com parametros PO'
        OnClick = SalvaPO1Click
      end
      object ResistenciaAberto01: TMenuItem
        Caption = 'Usar resistencia em aberto 1e09'
        OnClick = ResistenciaAberto01Click
      end
    end
    object Convergencia1: TMenuItem
      Caption = 'Op'#231#245'es de Converg'#234'ncia'
      OnClick = Convergencia1Click
    end
    object OpesdePlot1: TMenuItem
      Caption = 'Op'#231#245'es AC'
      OnClick = Plot1Click
    end
    object Sobre1: TMenuItem
      Caption = 'Sobre'
      OnClick = Sobre1Click
    end
  end
end
