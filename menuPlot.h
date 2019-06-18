//---------------------------------------------------------------------------

#ifndef menuPlotH
#define menuPlotH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TForm3 : public TForm
{
	__published:	// IDE-managed Components
	TLabel *Label1;
	TEdit *numPontosMenu;
	TLabel *Label2;
	TEdit *freInicialMenu;
	TLabel *Label3;
	TEdit *freFinalMenu;
	TLabel *Label4;
	TComboBox *ComboBox1;
	
	void __fastcall numPontosMenu1(TObject *Sender);
	void __fastcall freInicialMenu1(TObject *Sender);
	void __fastcall freFinalMenu1(TObject *Sender);
	void __fastcall ComboBox11(TObject *Sender);
	

private:	// User declarations
public:		// User declarations
	__fastcall TForm3(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm3 *Form3;
//---------------------------------------------------------------------------
#endif
